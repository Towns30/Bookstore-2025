#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <ranges>
#include <vector>

#include "BlockList.hpp"



TEST_CASE("BlockList Basic Insert", "[BlockList]") {
    std::filesystem::remove("data");
    BlockList<int, int> blocklist;
    blocklist.Initialise("data");
    auto data = std::vector<int>{6, 1, 2, 3, 4};
    const int key = 114514;
    for (auto v : data) blocklist.Insert(key, v);
    std::ranges::sort(data);
    auto result = blocklist.Find(key);
    // std::cerr << "Result:";
    // for (auto v:result) {
    //     std::cerr << v << ' ';
    // }
    // std::cerr << '\n';
    REQUIRE(result == data);
}

TEST_CASE("BlockList Massive Insert", "[BlockList]") {
    std::filesystem::remove("data");
    BlockList<int, unsigned> blocklist;
    blocklist.Initialise("data");
    const int N = 10000;
    std::vector<unsigned> data;
    std::mt19937 gen{14514};
    for (int i = 0; i < N; i++) {
        data.push_back(gen());
    }
    const int key = 114514;
    for (auto v : data) blocklist.Insert(key, v);
    std::ranges::sort(data);
    auto result = blocklist.Find(key);
    // std::cerr << "Result:";
    // for (auto v:result) {
    //     std::cerr << v << ' ';
    // }
    // std::cerr << '\n';
    REQUIRE(result == data);
}

TEST_CASE("BlockList Basic Delete", "[BlockList]") {
    std::filesystem::remove("data");
    BlockList<int, int> blocklist;
    blocklist.Initialise("data");
    auto data = std::vector<int>{6, 1, 2, 3, 4};
    const int key = 114514;
    for (auto v : data) blocklist.Insert(key, v);
    std::ranges::sort(data);
    auto result = blocklist.Find(key);
    REQUIRE(result == data);
    for (auto v : data) blocklist.Delete(key, v);
    result = blocklist.Find(key);
    REQUIRE(result == std::vector<int>{});
    for (auto v : data) blocklist.Insert(key, v);
    result = blocklist.Find(key);
    REQUIRE(result == data);
}

TEST_CASE("BlockList Delete Nonexist element", "[BlockList]") {
    std::filesystem::remove("data");
    BlockList<int, int> blocklist;
    blocklist.Initialise("data");
    for (int i = 1; i <= 10; i++) {
        blocklist.Insert(-i, -i);
    }
    std::mt19937 gen{14514};
    for (int i = 0; i < 10000; i++) {
        blocklist.Delete(gen() % 100000, gen() % 100000);
    }

    for (int i = 1; i <= 10; i++) {
        auto result = blocklist.Find(-i);
        REQUIRE(result == std::vector<int>{-i});
    }
}
TEST_CASE("BlockList Reopen", "[BlockList]") {
    std::filesystem::remove("data");
    std::vector<unsigned> data;
    const int N = 10000;
    std::mt19937 gen{14514};
    const int key = 114514;
    for (int i = 0; i < N; i++) {
        data.push_back(gen());
    }
    {
        BlockList<int, unsigned> blocklist;
        blocklist.Initialise("data");
        for (auto v : data) blocklist.Insert(key, v);
    }
    std::ranges::sort(data);
    {
        BlockList<int, unsigned> blocklist;
        blocklist.Initialise("data");
        auto result = blocklist.Find(key);
        REQUIRE(result == data);
    }
}