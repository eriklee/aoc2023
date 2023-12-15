#include "utils.hpp"

#include <algorithm>
#include <tuple>
#include <deque>
#include <fmt/format.h>
#include <numeric>
#include <vector>
#include <string_view>
#include <map>

using platform_t = std::vector<std::string>;

uint64_t scorePlatform(const platform_t& platform) {
  uint64_t result = 0;
  for (int i = 0; i < platform.size(); ++i) {
    auto rockCount = 0;
    for (const char c : platform[i]) { rockCount += c == 'O'; }
    // fmt::println("i={} row={} rc={}", i, platform[i], rockCount);
    result += rockCount * (platform.size() - i);
  }
  return result;
}

void tiltNorth(platform_t& platform) {
  const auto width = platform[0].size();
  const auto height = platform.size();

  for (int c = 0; c < width; ++c) {
    int nextEmpty = height + 1;
    for (int r = 0; r < height; ++r) {
      auto& tile = platform[r][c];
      switch (tile) {
        case '#': {
                    nextEmpty = height + 1;
                    continue;
                  }
        case '.': {
                    nextEmpty = std::min(r, nextEmpty);
                    continue;
                  }
        case 'O': {
                    if (r > nextEmpty) {
                      platform[nextEmpty++][c] = 'O';
                      tile = '.';
                    }
                  }
      }
    }
  }
}

void test() {
  platform_t testPlat = {
    "OOOO.#.O..",
    "OO..#....#",
    "OO..O##..O",
    "O..#.OO...",
    "........#.",
    "..#....#.#",
    "..O..#.O.O",
    "..O.......",
    "#....###..",
    "#....#....",
  };

  platform_t plat = {
"OOOO.#.O..",
"OO..#....#",
"OO..O##..O",
"O..#.OO...",
"........#.",
"..#....#.#",
"..O..#.O.O",
"..O.......",
"#....###..",
"#....#...."};
  utils::AssertEq(scorePlatform(plat), 136ul);
  tiltNorth(testPlat);
  utils::Assert(plat == testPlat);
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day14_test.txt"};
  utils::LineReader lr{"inp/day14.txt"};

  platform_t platform;
  while (auto line = lr.getLine()) {
      platform.push_back(std::string{*line});
  }
  tiltNorth(platform);
  uint64_t p1 = scorePlatform(platform);
  uint64_t p2 = 0;

  fmt::println("Day14: Part 1: {}", p1);
  // fmt::println("Day14: Part 2: {}", p2);
}
