#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "../src/Command.h"
#include <fmt/core.h>

TEST_CASE("Command test") {
  Command command("Забанить вакцинаторов на 3 недели");
  REQUIRE(command.isBan());
  REQUIRE(command.getDurationSec() == 60 * 60 * 24 * 7 * 3);
}

TEST_CASE("Storage test") {

  Storage<std::unordered_map<int, int>> originalStorage([](auto &map) {
    for (auto &&[_, v]: map) {
      ++v;
    }
  });

  originalStorage.use([](auto &map) {
    map.emplace(1, 2);
    map.emplace(2, 3);
  });

  originalStorage.update();
  originalStorage.update();

  originalStorage.use([](auto &map) {
    REQUIRE(map[1] == 4);
    REQUIRE(map[2] == 5);
  });

  auto serialized = originalStorage.serialize();

  Storage<std::unordered_map<int, int>> restoredStorage(serialized, nullptr);

  restoredStorage.use([](auto &map) {
    REQUIRE(map[1] == 4);
    REQUIRE(map[2] == 5);
  });
}


