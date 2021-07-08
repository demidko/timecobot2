#pragma once

#include <sw/redis++/redis++.h>
#include <unordered_map>
#include <mutex>

class Database {

  sw::redis::Redis redis;

  time_t lastSyncTimestamp = time(nullptr);

  std::mutex mutex;

public:
  std::unordered_map<int, int> timecoins;

  explicit Database(std::string_view);

  void sync();
};



