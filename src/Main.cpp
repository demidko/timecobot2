#include <tgbot/tgbot.h>
#include "Command.h"

/**
 * faq or status or cleaning
 * @param m message without reply to another message
 */
void checkSelfFeatures(const TgBot::Api &api, const TgBot::Message &m) {

}

/**
 *
 * @param api
 * @param m message with reply to another message
 */
void checkPinFeature(const TgBot::Api &api, const TgBot::Message &m) {
  if (auto maybePin = Command(m.text); maybePin.isPin()) {

  }
}

/**
 * transfer, ban, unban
 * @param m message with reply to another user
 */
void checkInteractiveFeatures(const TgBot::Api &api, const TgBot::Message &m) {

}

int main(int argc, char **argv) {
  auto bot = TgBot::Bot(getenv("TOKEN"));

  bot.getEvents().onAnyMessage([api = bot.getApi()](const TgBot::Message::Ptr &m) {
    if (!m->from || m->text.empty()) {
      return;
    }
    if (!m->replyToMessage) {
      return checkSelfFeatures(api, *m);
    }
    if (!m->replyToMessage->from) {
      return checkPinFeature(api, *m);
    }
    checkInteractiveFeatures(api, *m);
  });
  for (auto poll = TgBot::TgLongPoll(bot);;) {
    poll.start();
  }
}