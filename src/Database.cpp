#include "Database.h"

using namespace std;

namespace {

  constexpr uint8_t SYNC_PERIOD_SEC = 30;

  constexpr char TIMECOINS_KEY[] = "timecoins";
}

Database::Database(string_view url) : redis(url.data()) {
  lock_guard synchronized(mutex);

  redis.xread(TIMECOINS_KEY, "", UINT64_MAX, std::inserter(timecoins, timecoins.end()));
}

void Database::sync() {
  if (time(nullptr) - lastSyncTimestamp >= SYNC_PERIOD_SEC) {
    lock_guard synchronized(mutex);
    redis.hmset(TIMECOINS_KEY, timecoins.cbegin(), timecoins.cend());
    lastSyncTimestamp = time(nullptr);
  }
}
