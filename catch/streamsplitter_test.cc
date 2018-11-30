#include <catch.hpp>
#include <streamsplitter.cc>
#include <sstream>

TEST_CASE("split_by_line", "read from istringstream") {
    std::istringstream iss("a\nb\nc\nd\n", std::ios_base::in);

    auto split = split_by_line(iss);

    REQUIRE(split.size() == 5);
    REQUIRE(split[0] == "a");
    REQUIRE(split[1] == "b");
    REQUIRE(split[2] == "c");
    REQUIRE(split[3] == "d");
    REQUIRE(split[4] == "");
}