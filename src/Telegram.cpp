#include "Telegram.h"
#include <spdlog/spdlog.h>

using std::string_literals::operator ""s;

using namespace Telegram;

namespace {

const auto MUTED_PERMISSIONS = R"(
{
  "can_send_messages": false,
  "can_send_media_messages": false,
  "can_send_polls": false,
  "can_send_other_messages": false,
  "can_add_web_page_previews": false,
  "can_change_info": false,
  "can_invite_users": true,
  "can_pin_messages": false,
}
)";
}

Bot::Bot(std::string_view t) : url{"https://api.telegram.org/bot"s + t.data() + "/"} {}

void Bot::parseUpdates() {
  // todo periodically unpin deprecated messages
  auto updates = api("getUpdates", {
    {"timeout", "10"},
    {"allowed_updates", R"(["message","channel_post"])"},
    {"offset", std::to_string(updatesOffset)}
  });
  if (updates.error) {
    spdlog::critical("{}", updates.error.message);
    throw std::runtime_error(updates.error.message);
  }
  try {
    simdjson::padded_string json{updates.text};
    auto document = updatesParser.iterate(json);
    for (auto &&update: document["result"]) {
      updatesOffset = update["update_id"].get_int64() + 1;
      for (auto &&type: {"message", "channel_post"}) {
        if (parseMessage(update[type])) {
          break;
        }
      }
    }
  }
  catch (const simdjson::simdjson_error &e) {
    spdlog::error("{}\n{}\n{}", e.what(), updates.status_line, updates.text);
  }
}

bool Bot::parseMessage(simdjson::simdjson_result<simdjson::ondemand::value> container) {
  auto chatId = container["chat"]["id"].get_int64();
  if (chatId.error()) {
    return false;
  }
  auto messageId = container["message_id"].get_int64();
  auto senderId = container["from"]["id"].get_int64();

  if(messageId.error() || senderId.error()) {
    return false;
  }

  // todo delete muted messages

  auto fileId = container["document"]["file_id"].get_string();
  if (chatFileHandler && !fileId.error()) {
    threadPool.enqueue(
      [
        this,
        file = ChatFile{
          .chatId = chatId.value(),
          .fileId = std::string{fileId.value()}
        }
      ] {
        chatFileHandler(*this, file);
      });
    return true;
  }
  auto text = container["text"].get_string();
  if (text.error()) {
    return false;
  }
  auto recipientId = container["reply_to_message"]["from"]["id"].get_int64();
  auto recipientMessageId = container["reply_to_message"]["message_id"].get_int64();
  if (chatInteractionHandler && !recipientId.error() && !recipientMessageId.error()) {
    threadPool.enqueue(
      [
        this,
        interaction = ChatInteraction{
          .chatId = chatId.value(),
          .senderId = senderId.value(),
          .text = std::string{text.value()},
          .recipientId = recipientId.value(),
          .recipientMessageId = recipientMessageId.value()
        }
      ] {
        chatInteractionHandler(*this, interaction);
      }
    );
    return true;
  }
  if (chatActionHandler) {
    threadPool.enqueue(
      [
        this,
        action = ChatAction{
          .chatId = chatId.value(),
          .senderId = senderId.value(),
          .text = std::string{text.value()},
          .messageId = messageId.value()
        }
      ] {
        chatActionHandler(*this, action);
      }
    );
    return true;
  }
  return false;
}

void Bot::run() {
  for (;; parseUpdates());
}

void Bot::reply(ChatId chatId, MessageId messageId, std::string_view reply) const {
  api("sendMessage", {
    {"chat_id", std::to_string(chatId)},
    {"reply_to_message_id", std::to_string(messageId)},
    {"text", reply.data()}
  });
}

Bot &Bot::onChatAction(ChatActionHandler h) {
  chatActionHandler = move(h);
  return *this;
}

Bot &Bot::onChatInteraction(ChatInteractionHandler h) {
  chatInteractionHandler = move(h);
  return *this;
}

cpr::Response Bot::api(std::string_view method, const cpr::Parameters &p) const {
  return cpr::Get(url + method.data(), p, cpr::VerifySsl{false});
}

Bot &Bot::OnChatFile(ChatFileHandler h) {
  chatFileHandler = move(h);
  return *this;
}

std::unordered_map<ChatId, std::unordered_map<UserId, time_t>> Bot::getMutedAdmins() const {
  return mutedAdmins;
}

std::unordered_map<ChatId, std::unordered_map<MessageId, time_t>> Bot::getPinnedMessages() const {
  return pinnedMessages;
}

bool Bot::mute(ChatId chatId, UserId userId, time_t untilSecond) {
  cpr::Parameter chat{"chat_id", std::to_string(chatId)};
  auto admins = api("getChatAdministrators", {chat});
  if (!admins.error) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string json{admins.text};
    auto array = parser.iterate(json);
    for (auto &&admin: array) {
      if (auto id = admin["user"]["id"].get_int64(); !id.error() && id.value() == userId) {
        std::lock_guard guard{mutedAdminsMutex};
        std::unordered_map<UserId, time_t> &chatDb = mutedAdmins[chatId];
        if (auto it = chatDb.find(userId); it == chatDb.cend()) {
          chatDb[userId] = untilSecond;
        } else {
          it->second = untilSecond + (untilSecond - it->second);
        }
        return true;
      }
    }
  }
  auto result = api("restrictChatMember", {
    chat,
    {"user_id", std::to_string(userId)},
    {"until_date", std::to_string(untilSecond)},
    {"permissions", MUTED_PERMISSIONS}
  });
  return result.text == "True";
}

bool Bot::unmute(ChatId chatId, UserId userId) {
  auto chatDb = mutedAdmins[chatId];
  if (auto it = chatDb.find(userId); it != chatDb.cend()) {
    std::lock_guard guard{mutedAdminsMutex};
    chatDb.erase(it);
    return true;
  }
  auto result = api("restrictChatMember", {
    {"chat_id", std::to_string(chatId)},
    {"user_id", std::to_string(userId)},
    {"permissions", "True"}
  });
  return result.text == "True";
}

bool Bot::pin(ChatId chatId, MessageId messageId, time_t untilSec) {
  {
    std::lock_guard guard{pinnedMessagesMutex};
    auto chatDb = pinnedMessages[chatId];
    if (auto it = chatDb.find(messageId); it == chatDb.cend()) {
      chatDb[messageId] = untilSec;
    } else {
      it->second = untilSec + (untilSec - it->second);
    }
  }
  auto result = api("pinChatMessage", {
    {"chat_id", std::to_string(chatId)},
    {"message_id", std::to_string(messageId)},
  });
  return result.text == "True";
}
