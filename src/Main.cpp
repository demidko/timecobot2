#include <iostream>
#include <tgbot/tgbot.h>
#include <fmt/core.h>

int main(int argc, char **argv) {
  auto bot = TgBot::Bot(getenv("TOKEN"));
  bot.getEvents().onAnyMessage([api = bot.getApi()](const TgBot::Message::Ptr &m) {
    if (!m->replyToMessage) {
      fmt::print("no-reply: {}\n", m->text);
      return;
    }
    if (!m->replyToMessage->from) {
      fmt::print("no-reply-sender: {}\n", m->text);
    }
    api.sendMessage(m->chat->id, "It works", false, m->replyToMessage->from->id);
  });
  auto longPoll = TgBot::TgLongPoll(bot);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  while (true) {
    fmt::print("Updated\n");
    longPoll.start();
  }
#pragma clang diagnostic pop
}