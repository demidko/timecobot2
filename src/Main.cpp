#include <iostream>
#include <tgbot/tgbot.h>
#include <fmt/core.h>
#include <fmt/color.h>

/**
 * faq or status
 * @param m message without reply to another message
 */
void checkFaqOrStatus(const TgBot::Api &api, const TgBot::Message &m) {

}

/**
 *
 * @param api
 * @param m message with reply to another message
 */
void checkPin(const TgBot::Api &api, const TgBot::Message &m) {

}

/**
 * transfer, ban, unban
 * @param m message with reply to another user
 */
void checkBanOrUnbanOrTransferOrPin(const TgBot::Api &api, const TgBot::Message &m) {

}

int main(int argc, char **argv) {
  auto bot = TgBot::Bot(getenv("TOKEN"));
  bot.getEvents().onAnyMessage([api = bot.getApi()](const TgBot::Message::Ptr &m) {
    if (m->text.empty()) {
      return;
    }
    if (!m->replyToMessage) {
      return checkFaqOrStatus(api, *m);
    }
    if (!m->replyToMessage->from) {
      return checkPin(api, *m);
    }
    checkBanOrUnbanOrTransferOrPin(api, *m);
  });


#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  for (auto longPoll = TgBot::TgLongPoll(bot);;) longPoll.start();
#pragma clang diagnostic pop
}