#include <catch.hpp>
#include <streamsplitter.cc>
#include <sstream>

using namespace cmp;

TEST_CASE("split_by_line", "read from istringstream") {
    std::istringstream iss("a\nb\nc\nd\n", std::ios_base::in);

    auto split = split_by_line(iss);

    CHECK(split.size() == 4);
    CHECK(split[0] == "a\n");
    CHECK(split[1] == "b\n");
    CHECK(split[2] == "c\n");
    CHECK(split[3] == "d\n");
}

TEST_CASE("split_by_character", "read from istringstream") {
    std::istringstream iss("a\n\03b", std::ios_base::in);

    auto split = split_by_character(iss);

    CHECK(split.size() == 4);
    CHECK(split[0] == 'a');
    CHECK(split[1] == '\n');
    CHECK(split[2] == '\03');
    CHECK(split[3] == 'b');
}

TEST_CASE("split_by_token", "read from istringstream") {
    std::istringstream iss("abc  \tm$_<>(~fg 2f_ \n\n;^(*");

    auto split = split_by_token(iss);

    CHECK(split.size() == 14);
    CHECK(split[0] == "abc");
    CHECK(split[1] == " ");
    CHECK(split[2] == " ");
    CHECK(split[3] == "\t");
    CHECK(split[4] == "m$_");
    CHECK(split[5] == "<>(~");
    CHECK(split[6] == "fg");
    CHECK(split[7] == " ");
    CHECK(split[8] == "2f_");
    CHECK(split[9] == " ");
    CHECK(split[10] == "\n");
    CHECK(split[11] == "\n");
    CHECK(split[12] == ";^(*");
    CHECK(split[13] == "\n");
}

TEST_CASE("split_by_line ignore crlf true", "read from istringstream") {
    std::istringstream iss("a\r\nb\r\nc\r\nd\r\n", std::ios_base::in);

    auto split = split_by_line(iss,true);

    REQUIRE(split.size() == 4);
    CHECK(split[0] == "a\n");
    CHECK(split[1] == "b\n");
    CHECK(split[2] == "c\n");
    CHECK(split[3] == "d\n");
}

TEST_CASE("split_by_line ignore crlf false", "read from istringstream") {
    std::istringstream iss("a\r\nb\r\nc\r\nd\r\n", std::ios_base::in);

    auto split = split_by_line(iss,false);

    REQUIRE(split.size() == 4);
    CHECK(split[0] == "a\r\n");
    CHECK(split[1] == "b\r\n");
    CHECK(split[2] == "c\r\n");
    CHECK(split[3] == "d\r\n");
}

TEST_CASE("split_by_character ignore crlf true", "read from istringstream") {
    std::istringstream iss("a\r\n\03b", std::ios_base::in);

    auto split = split_by_character(iss, true);

    REQUIRE(split.size() == 4);
    CHECK(split[0] == 'a');
    CHECK(split[1] == '\n');
    CHECK(split[2] == '\03');
    CHECK(split[3] == 'b');
}

TEST_CASE("split_by_character ignore crlf false", "read from istringstream") {
    std::istringstream iss("a\r\n\03b", std::ios_base::in);

    auto split = split_by_character(iss, false);

    REQUIRE(split.size() == 5);
    CHECK(split[0] == 'a');
    CHECK(split[1] == '\r');
    CHECK(split[2] == '\n');
    CHECK(split[3] == '\03');
    CHECK(split[4] == 'b');
}

TEST_CASE("split_by_token crlf ignore true", "read from istringstream") {
    std::istringstream iss("abc\r\n;^(*");

    auto split = split_by_token(iss, true);

    REQUIRE(split.size() == 4);
    CHECK(split[0] == "abc");
    CHECK(split[1] == "\n");
    CHECK(split[2] == ";^(*");
    CHECK(split[3] == "\n");
}

TEST_CASE("split_by_token ignore crlf false", "read from istringstream") {
    std::istringstream iss("abc\r\n;^(*");

    auto split = split_by_token(iss, false);

    REQUIRE(split.size() == 5);
    CHECK(split[0] == "abc");
    CHECK(split[1] == "\r");
    CHECK(split[2] == "\n");
    CHECK(split[3] == ";^(*");
    CHECK(split[4] == "\n");
}