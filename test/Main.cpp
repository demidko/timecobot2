#define CATCH_CONFIG_MAIN

#include "../src/Command.h"
#include <catch2/catch.hpp>
#include <iostream>
#include <unicode/std_string.h>
#include <unicode/urename.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/utypes.h>
#include <unicode/stringoptions.h>
#include <unicode/ucpmap.h>
#include <unicode/uchar.h>
#include <unicode/numfmt.h>

TEST_CASE("Command test") {
  Command command("Забанить вакцинаторов на 3 недели");
  REQUIRE(command.isBan());
  REQUIRE(command.getDurationSec() == 60 * 60 * 24 * 7 * 3);
}

