#pragma once

#include <functional>
#include <string_view>
#include <cpr/cpr.h>
#include <simdjson.h>
#include <ThreadPool/ThreadPool.h>
#include <variant>

namespace Telegram {

  class Bot;

  struct ChatAction {
    int64_t chatId;
    int64_t sender;
    int64_t senderMessage;
  };

  using ChatActionHandler = std::function<void(const Bot &, const ChatAction &)>;

  struct ChatInteraction {
    int64_t chatId;
    int64_t sender;
    int64_t recipient;
    int64_t recipientMessage;
  };

  using ChatInteractionHandler = std::function<void(const Bot &, const ChatInteraction &)>;

  class Bot {
    const cpr::Url url;
    ChatActionHandler chatActionHandler;
    ChatInteractionHandler chatInteractionHandler;
    simdjson::ondemand::parser parser;
    const cpr::Url getUpdates = url + "getUpdates";
    const cpr::Url sendMessage = url + "sendMessage";
    const cpr::Parameter timeout{"timeout", "10"};
    const cpr::Parameter allowedUpdates{"allowed_updates", R"(["message","channel_post"])"};
    const std::string messageTypes[2]{"message", "channel_post"};
    int64_t offset{};
    ThreadPool pool{4};
    const cpr::VerifySsl ssl{false};

    bool handleMessage(simdjson::simdjson_result<simdjson::ondemand::value> container);

    void handleUpdates();

    cpr::Response request(std::string_view, const cpr::Parameters&);

  public:

    explicit Bot(std::string_view);

    Bot &onChatAction(ChatActionHandler);

    Bot &onChatInteraction(ChatInteractionHandler);

    [[noreturn]] void run();

    void reply(int64_t chat, int64_t originalMessage, std::string_view reply) const;
  };
}