#pragma once
#include <functional>
#include <string_view>
#include <cpr/cpr.h>
#include <simdjson.h>
#include <ThreadPool/ThreadPool.h>
#include <unordered_map>
#include <mutex>

namespace Telegram {

using ChatId = int64_t;

using UserId = int64_t;

using MessageId = int64_t;

class Bot;

struct ChatAction {
  ChatId chatId;
  UserId senderId;
  std::string text;
  UserId messageId;
};

using ChatActionHandler = std::function<void(Bot &, const ChatAction &)>;

struct ChatInteraction {
  ChatId chatId;
  UserId senderId;
  std::string text;
  UserId recipientId;
  MessageId recipientMessageId;
};

using ChatInteractionHandler = std::function<void(Bot &, const ChatInteraction &)>;

struct ChatFile {
  ChatId chatId;
  std::string fileId;
};

using ChatFileHandler = std::function<void(const Bot &, const ChatFile &)>;

class Bot {
  const cpr::Url url;
  ChatActionHandler chatActionHandler;
  ChatInteractionHandler chatInteractionHandler;
  ChatFileHandler chatFileHandler;
  simdjson::ondemand::parser updatesParser;
  int64_t updatesOffset{};
  ThreadPool threadPool{4};
  std::unordered_map<ChatId, std::unordered_map<UserId, time_t>> mutedAdmins;
  std::mutex mutedAdminsMutex;
  std::unordered_map<ChatId, std::unordered_map<MessageId, time_t>> pinnedMessages;
  std::mutex pinnedMessagesMutex;

  bool parseMessage(simdjson::simdjson_result<simdjson::ondemand::value>);

  void parseUpdates();

  [[nodiscard]] cpr::Response api(std::string_view method, const cpr::Parameters &p) const;

public:
  explicit Bot(std::string_view);

  Bot &onChatAction(ChatActionHandler);

  Bot &OnChatFile(ChatFileHandler);

  Bot &onChatInteraction(ChatInteractionHandler);

  [[noreturn]] void run();

  [[nodiscard]]
  std::unordered_map<ChatId, std::unordered_map<UserId, time_t>> getMutedAdmins() const;

  void muteAllAdmins(const std::unordered_map<ChatId, std::unordered_map<UserId, time_t>> &);

  [[nodiscard]]
  std::unordered_map<ChatId, std::unordered_map<MessageId, time_t>> getPinnedMessages() const;

  void pinAllMessages(const std::unordered_map<ChatId, std::unordered_map<MessageId, time_t>> &);

  bool pin(ChatId, MessageId, time_t) const;

  bool unpin(ChatId, MessageId) const;

  bool unmute(ChatId, UserId);

  bool mute(ChatId, UserId, time_t);

  void reply(ChatId, MessageId, std::string_view) const;
};
}