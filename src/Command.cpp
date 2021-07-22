#include "Command.h"
#include <unicode/utypes.h>
#include <unicode/uchar.h>
#include <unicode/numfmt.h>

using namespace icu;
using namespace std;

namespace {

  constexpr uint8_t MINUTE_SECONDS = 60;
  constexpr uint16_t HOUR_SECONDS = MINUTE_SECONDS * 60;
  constexpr uint32_t DAY_SECONDS = HOUR_SECONDS * 24;
  constexpr uint32_t WEEK_SECONDS = DAY_SECONDS * 7;
  constexpr uint32_t MONTH_SECONDS = DAY_SECONDS * 30;
  constexpr uint32_t YEAR_SECONDS = DAY_SECONDS * 365;
  constexpr uint8_t MIN_SECONDS = 30;
  constexpr uint32_t MAX_SECONDS = DAY_SECONDS * 366;

  UnicodeString toLowerTrimmed(string_view text) {
    return UnicodeString(text.data()).trim().toLower();
  }
}

Command::Command(string_view data) : text(toLowerTrimmed(data)) {}

bool Command::isSeparator(int32_t i) const {
  return !u_isalpha(text[i]);
}

int32_t Command::indexOfStem(string_view stem) const {
  auto idx = text.indexOf(toLowerTrimmed(stem));
  if (idx == -1 || (idx > 0 && !isSeparator(idx - 1))) {
    return -1;
  }
  return idx;
}

int32_t Command::indexOfWord(string_view word) const {
  auto idx = text.indexOf(toLowerTrimmed(word));
  if (idx == -1 || (idx > 0 && !isSeparator(idx - 1))) {
    return -1;
  }
  if (auto next = idx + word.length(); next < text.length() && !isSeparator(int32_t(next))) {
    return -1;
  }
  return idx;
}

bool Command::containsStem(string_view s) const {
  return indexOfStem(s) != -1;
}

bool Command::containsWord(string_view w) const {
  return indexOfWord(w) != -1;
}

bool Command::startsWithStem(string_view s) const {
  return indexOfStem(s) == 0;
}

bool Command::startsWithWord(string_view w) const {
  return indexOfWord(w) == 0;
}

uint32_t Command::getTimeUnitSec() const {
  if (containsStems("sec", "сек") || containsWords("s", "с")) {
    return 1;
  }
  if (containsStems("min", "мин") || containsWords("m", "м")) {
    return MINUTE_SECONDS;
  }
  if (containsStems("час", "hour") || containsWords("h", "ч")) {
    return HOUR_SECONDS;
  }
  if (containsStems("day", "суток", "сутк",
                    "дня", "ден", "дне") ||
      containsWords("d", "д")) {
    return DAY_SECONDS;
  }
  if (containsStems("week", "недел")) {
    return WEEK_SECONDS;
  }
  if (containsStems("mon", "mo", "мес")) {
    return MONTH_SECONDS;
  }
  if (containsStems("year", "год", "лет") ||
      containsWords("y", "yr", "г", "л")) {
    return YEAR_SECONDS;
  }
  return MINUTE_SECONDS;
}

uint64_t Command::getNumber() const {
  auto firstDigitIdx = indexOf(u_isdigit);
  if (firstDigitIdx == -1) {
    return 1;
  }
  auto nextNonDigitIdx = indexOf(not_fn(u_isdigit), firstDigitIdx);
  auto lastDigitIdx = nextNonDigitIdx == -1 ? text.length() : nextNonDigitIdx;
  string digits;
  text.tempSubStringBetween(firstDigitIdx, lastDigitIdx).toUTF8String(digits);
  return stoull(digits);
}

uint64_t Command::getDurationSec() const {
  auto seconds = getTimeUnitSec() * getNumber();
  if (seconds < MIN_SECONDS) {
    return MIN_SECONDS;
  }
  if (seconds > MAX_SECONDS) {
    return MAX_SECONDS;
  }
  return seconds;
}

bool Command::isBan() const {
  return startsWithStems(
    "ban",
    "block",
    "freez",
    "mute",
    "бан",
    "блок",
    "забан",
    "заглох",
    "завали",
    "умри",
    "умер",
    "мьют",
    "замьют",
    "💥");
}

bool Command::isUnban() const {
  return startsWithStems(
    "liberat",
    "heal",
    "ransom",
    "atonement",
    "expiation",
    "redemption",
    "rescue",
    "salvation",
    "redeem",
    "unblock",
    "unban",
    "unmute",
    "разбан",
    "разблок",
    "ожив",
    "выкуп",
    "исцел",
    "искуп",
    "спаст",
    "освобод");
}

bool Command::isTransfer() const {
  return
    startsWithWords("дать") ||
    startsWithStems(
      "transfer",
      "give",
      "take",
      "get",
      "keep",
      "держи",
      "бери",
      "возьми",
      "трансфер",
      "перевод",
      "дар",
      "подар",
      "взял",
      "забер",
      "забир",
      "перевед",
      "перевест",
      "отправ",
      "send");
}

bool Command::isPin() const {
  return
    startsWithWords("пин") ||
    startsWithStems("закреп", "pin", "запин");
}

bool Command::isFaq() const {
  return startsWithStems(
    "помощ",
    "справк",
    "правил",
    "help",
    "rule",
    "faq",
    "onUpdateAction",
    "старт"
  );
}

bool Command::isBalance() const {
  return startsWithStems(
    "time",
    "врем",
    "balance",
    "status",
    "score",
    "coins",
    "баланс",
    "статус",
    "счет",
    "счёт",
    "узна",
    "timecoin",
    "check"
  );
}

bool Command::isCleaning() const {
  return startsWithStems(
    "autoclean",
    "автоочист",
    "шухер",
    "палево"
  );
}
