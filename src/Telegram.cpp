#include "Telegram.h"
#include <spdlog/spdlog.h>

using std::string_literals::operator ""s;

Telegram::Bot::Bot(std::string_view t) : url{"https://api.telegram.org/bot"s + t.data() + "/"} {}

void Telegram::Bot::handleUpdates() {
  auto response = cpr::Get(
    getUpdates,
    cpr::Parameters{
      timeout,
      allowedUpdates,
      {"offset", std::to_string(offset)}
    },
    ssl
  );
  if (response.error) {
    spdlog::critical("{}", response.error.message);
    throw std::runtime_error(response.error.message);
  }
  try {
    simdjson::padded_string json = response.text;
    auto document = parser.iterate(json);
    for (auto update: document["result"]) {
      offset = update["update_id"].get_int64() + 1;
      for (const auto &type: messageTypes) {
        if (handleMessage(update[type])) {
          break;
        }
      }
    }
  } catch (const simdjson::simdjson_error &e) {
    spdlog::error("{}\n{}\n{}", response.status_line, e.what(), response.text);
  }
}

bool Telegram::Bot::handleMessage(simdjson::simdjson_result<simdjson::ondemand::value> message) {
  auto chatId = message["chat"]["id"].get_int64();
  if (chatId.error()) {
    return false;
  }
  if (fileHandler != nullptr) {
    auto fileId = message["document"]["file_id"].get_string();
    auto caption = message["caption"].get_string();
    if (!fileId.error() && !caption.error() && !caption.value().empty()) {
      pool.enqueue(
        [
          this,
          file = File{chatId.value(), std::string{caption.value()}, std::string{fileId.value()}}
        ] {
          fileHandler(*this, file);
        });
      return true;
    }
  }
  if (requestHandler != nullptr) {
    auto text = message["text"].get_string();
    auto messageId = message["message_id"].get_int64();
    if (!text.error() && !messageId.error() && !text.value().empty()) {
      pool.enqueue(
        [
          this,
          request = Request{chatId.value(), messageId.value(), std::string{text.value()}}
        ] {
          requestHandler(*this, request);
        });
      return true;
    }
  }
  return false;
}

[[noreturn]] void Telegram::Bot::run() {
  for (;; handleUpdates());
}


void Telegram::Bot::reply(int64_t chat, int64_t originalMessage, std::string_view reply) const {
  cpr::Get(
    sendMessage,
    cpr::Parameters{
      {"chat_id",             std::to_string(chat)},
      {"reply_to_message_id", std::to_string(originalMessage)},
      {"text",                reply.data()}
    },
    ssl
  );
}

Telegram::Bot &Telegram::Bot::onChatAction(Telegram::ChatActionHandler h) {
  chatActionHandler = move(h);
  return *this;
}

Telegram::Bot &Telegram::Bot::onChatInteraction(Telegram::ChatInteractionHandler h) {
  chatInteractionHandler = move(h);
  return *this;
}

cpr::Response Telegram::Bot::request(std::string_view method, const cpr::Parameters &p) {
  return cpr::Get(url + method.data(), p, cpr::VerifySsl{false});
}
