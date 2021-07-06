#include <iostream>
#include <tgbot/tgbot.h>
#include <fmt/core.h>
#include <fmt/color.h>

int main(int argc, char **argv) {
  auto bot = TgBot::Bot(getenv("TOKEN"));
  bot.getEvents().onAnyMessage([api = bot.getApi()](const TgBot::Message::Ptr &m) {
    fmt::print("chat: {}, from: {}\n", m->chat->id, m->from->id);

    //api.sendMessage(m->chat->id, "It works", false, m->replyToMessage->from->id);
  });


#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  for (auto longPoll = TgBot::TgLongPoll(bot);;) longPoll.start();
#pragma clang diagnostic pop
}