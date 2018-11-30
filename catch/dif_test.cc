#include "dif.cc" //cc since I am testing the static functions

#include <catch.hpp>


TEST_CASE("finalize_dif - preserve", "no changes") {
    dif_hist dh;//empty for no changes
    dif<std::string> d = finalize_dif<std::string>(dh,"abcd","abcd");

    CHECK(d.ds.size() == 1);
    auto it = d.ds.begin();

    CHECK(it->type == dif_segment<std::string>::preserved);
    CHECK(it->s == "abcd");
}

TEST_CASE("finalize_dif - delete", "deletioin, no insetion") {
    dif_hist dh{{dif_hist::del},{},0};
    dif<std::string> d = finalize_dif<std::string>(dh,"abcd","abd");

    CHECK(d.ds.size() == 3);
    auto it = d.ds.begin();

    CHECK(it->type == dif_segment<std::string>::preserved);
    CHECK(it->s == "ab");
    ++it;

    CHECK(it->type == dif_segment<std::string>::deletion);
    CHECK(it->s == "c");
    ++it;

    CHECK(it->type == dif_segment<std::string>::preserved);
    CHECK(it->s == "d");
}

TEST_CASE("finalize_dif - insert", "insertion, no deletion") {
    dif_hist dh{{dif_hist::ins},{},0};
    dif<std::string> d = finalize_dif<std::string>(dh,"abcd","aXbcd");

    CHECK(d.ds.size() == 3);
    auto it = d.ds.begin();

    CHECK(it->type == dif_segment<std::string>::preserved);
    CHECK(it->s == "a");
    ++it;

    CHECK(it->type == dif_segment<std::string>::insertion);
    CHECK(it->s == "X");
    ++it;

    CHECK(it->type == dif_segment<std::string>::preserved);
    CHECK(it->s == "bcd");

}

TEST_CASE("finalize_dif - modify", "insertion and deletion") {
    dif_hist dh{{dif_hist::ins,dif_hist::del},{},0};
    dif<std::string> d = finalize_dif<std::string>(dh,"abcd","aXbd");

    CHECK(d.ds.size() == 5);
    auto it = d.ds.begin();

    CHECK(it->type == dif_segment<std::string>::preserved);
    CHECK(it->s == "a");
    ++it;

    CHECK(it->type == dif_segment<std::string>::insertion);
    CHECK(it->s == "X");
    ++it;

    CHECK(it->type == dif_segment<std::string>::preserved);
    CHECK(it->s == "b");
    ++it;

    CHECK(it->type == dif_segment<std::string>::deletion);
    CHECK(it->s == "c");
    ++it;

    CHECK(it->type == dif_segment<std::string>::preserved);
    CHECK(it->s == "d");
}

TEST_CASE("finalize_dif - replace", "abcdefg -> abcdXfg.") {
    dif_hist dh{{dif_hist::del,dif_hist::ins},{7,7},0};
    dif<std::string> d = finalize_dif<std::string>(dh,"abcdefg","abcdXfg");

    CHECK(d.ds.size() == 4);
    auto it = d.ds.begin();

    CHECK(it->type == dif_segment<std::string>::preserved);
    CHECK(it->s == "abcd");
    ++it;

    CHECK(it->type == dif_segment<std::string>::deletion);
    CHECK(it->s == "e");
    ++it;

    CHECK(it->type == dif_segment<std::string>::insertion);
    CHECK(it->s == "X");
    ++it;

    CHECK(it->type == dif_segment<std::string>::preserved);
    CHECK(it->s == "fg");
}

TEST_CASE("paths_at_end", "abcd -> abd: only 4,3 works") {
    std::string original = "abcd", final = "abd";
    std::vector<dif_hist> paths;
    paths.push_back(dif_hist{{},{3,4},0});
    paths.push_back(dif_hist{{},{4,3},0});
    paths.push_back(dif_hist{{},{4,4},0});
    paths.push_back(dif_hist{{},{3,3},0});
    paths.push_back(dif_hist{{},{0,0},0});
    paths.push_back(dif_hist{{},{5,5},0});
    
    
    auto end_paths = paths_at_end(paths, original.size(), final.size());

    REQUIRE(end_paths.size() == 1);

    REQUIRE(end_paths.front()->current == dif_point{4,3});
}

