#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "../src/Command.h"
#include <fmt/core.h>

TEST_CASE("Command test") {
  Command command{"Забанить вакцинаторов на 3 недели"};
  REQUIRE(command.isBan());
  REQUIRE(command.getDurationSec() == 60 * 60 * 24 * 7 * 3);
}
