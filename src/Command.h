#pragma once

#include <string_view>
#include <unicode/unistr.h>

class Command {

  const icu::UnicodeString text;

  [[nodiscard]] bool isSeparator(int32_t) const;

  template<class Predicate>
  [[nodiscard]] int32_t indexOf(const Predicate &predicate, int32_t from = 0) const {
    for (int32_t i = from; i < text.length(); ++i) {
      if (predicate(text[i])) {
        return i;
      }
    }
    return -1;
  }

  [[nodiscard]] int32_t indexOfStem(std::string_view) const;

  [[nodiscard]] bool containsStem(std::string_view) const;

  [[nodiscard]] bool startsWithStem(std::string_view) const;

  [[nodiscard]] int32_t indexOfWord(std::string_view) const;

  [[nodiscard]] bool containsWord(std::string_view) const;

  [[nodiscard]] bool startsWithWord(std::string_view) const;

  template<class ...Stem>
  [[nodiscard]] bool containsStems(const Stem &... stems) const {
    for (auto &&s: {stems...}) {// NOLINT(readability-use-anyofallof)
      if (containsStem(s)) {
        return true;
      }
    }
    return false;
  }

  template<class ...Word>
  [[nodiscard]] bool containsWords(const Word &... words) const {
    for (auto &&w: {words...}) { // NOLINT(readability-use-anyofallof)
      if (containsWord(w)) {
        return true;
      }
    }
    return false;
  }

  [[nodiscard]] uint32_t getTimeUnitSec() const;

  [[nodiscard]] uint64_t getNumber() const;

  template<class ...Stem>
  [[nodiscard]] bool startsWithStems(const Stem &... stems) const {
    for (auto &&s: {stems...}) { // NOLINT(readability-use-anyofallof)
      if (indexOfStem(s) == 0) {
        return true;
      }
    }
    return false;
  }

  template<class ...Word>
  [[nodiscard]] bool startsWithWords(const Word &... words) const {
    for (auto &&w: {words...}) { // NOLINT(readability-use-anyofallof)
      if (indexOfWord(w) == 0) {
        return true;
      }
    }
    return false;
  }

public:
  explicit Command(std::string_view);

  [[nodiscard]] bool isBan() const;

  [[nodiscard]] bool isUnban() const;

  [[nodiscard]] bool isTransfer() const;

  [[nodiscard]] bool isPin() const;

  [[nodiscard]] bool isFaq() const;

  [[nodiscard]] bool isBalance() const;

  [[nodiscard]] bool isCleaning() const;

  [[nodiscard]] uint64_t getDurationSec() const;
};



