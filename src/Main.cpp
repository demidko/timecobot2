#include <iostream>
#include <tgbot/tgbot.h>

int main(int argc, char **argv) {
  auto bot = TgBot::Bot(getenv("TOKEN"));
  bot.getEvents().onAnyMessage([api = bot.getApi()](const TgBot::Message::Ptr &m) {
    if (auto sourceMessage = m->replyToMessage; sourceMessage) {
      if (auto sender = sourceMessage->from; sender) {
        api.sendMessage(m->chat->id, "It depends", false, sender->id);
      }
    }
  });
  auto longPoll = TgBot::TgLongPoll(bot);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  while (true) {
    longPoll.start();
  }
#pragma clang diagnostic pop

}