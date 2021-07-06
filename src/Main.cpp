#include <iostream>
#include <tgbot/tgbot.h>

int main(int argc, char **argv) {

  TgBot::Bot bot(getenv("TOKEN"));
  std::cout << bot.getApi().getMe()->username;
  /*bot.getEvents().onAnyMessage([&bot](const TgBot::Message::Ptr &message) {

  });
  TgBot::TgLongPoll(bot).start();*/
}