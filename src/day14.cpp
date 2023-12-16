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


template<bool Debug=false>
uint64_t scorePlatform(const platform_t& platform) {
  uint64_t result = 0;
  for (int i = 0; i < platform.size(); ++i) {
    auto rockCount = 0;
    for (const char c : platform[i]) { rockCount += c == 'O'; }
    if constexpr (Debug) fmt::println("i={} row={} rc={}", i, platform[i], rockCount);
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

void tiltSouth(platform_t& platform) {
  const auto width = platform[0].size();
  const auto height = platform.size();

  for (int c = 0; c < width; ++c) {
    int nextEmpty = -1;
    for (int r = height - 1; r >= 0; --r) {
      auto& tile = platform[r][c];
      switch (tile) {
        case '#': {
                    nextEmpty = -1;
                    continue;
                  }
        case '.': {
                    nextEmpty = std::max(r, nextEmpty);
                    continue;
                  }
        case 'O': {
                    if (r < nextEmpty) {
                      platform[nextEmpty--][c] = 'O';
                      tile = '.';
                    }
                  }
      }
    }
  }
}

void tiltWest(platform_t& platform) {
  const auto width = platform[0].size();

  for (auto& row : platform) {
    int nextEmpty = width + 1;
    for (int c = 0; c < width; ++c) {
      auto& tile = row[c];
      switch (tile) {
        case '#': {
                    nextEmpty = width + 1;
                    continue;
                  }
        case '.': {
                    nextEmpty = std::min(c, nextEmpty);
                    continue;
                  }
        case 'O': {
                    if (c > nextEmpty) {
                      row[nextEmpty++] = 'O';
                      tile = '.';
                    }
                  }
      }
    }
  }
}

void tiltEast(platform_t& platform) {
  const auto width = platform[0].size();

  for (auto& row : platform) {
    int nextEmpty = -1;
    for (int c = width - 1; c >= 0; --c) {
      auto& tile = row[c];
      switch (tile) {
        case '#': {
                    nextEmpty = -1;
                    continue;
                  }
        case '.': {
                    nextEmpty = std::max(c, nextEmpty);
                    continue;
                  }
        case 'O': {
                    if (c < nextEmpty) {
                      row[nextEmpty--] = 'O';
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

  auto northTip = testPlat;
  tiltSouth(testPlat);
  tiltNorth(testPlat);
  utils::Assert(northTip == testPlat);
}

template<bool Debug = false>
void spinCycle(platform_t& platform) {
  tiltNorth(platform);
  if constexpr (Debug) {
    fmt::println("Tilted NORTH");
    scorePlatform<Debug>(platform);
  }
  tiltWest(platform);
  if constexpr (Debug) {
    fmt::println("Tilted WEST");
    scorePlatform<Debug>(platform);
  }
  tiltSouth(platform);
  if constexpr (Debug) {
    fmt::println("Tilted SOUTH");
    scorePlatform<Debug>(platform);
  }
  tiltEast(platform);
  if constexpr (Debug) {
    fmt::println("Tilted EAST");
    scorePlatform<Debug>(platform);
  }
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day14_test.txt"};
  utils::LineReader lr{"inp/day14.txt"};

  platform_t platform;
  while (auto line = lr.getLine()) {
      platform.push_back(std::string{*line});
  }
  auto lastPlatform = platform;
  tiltNorth(platform);
  uint64_t p1 = scorePlatform(platform);

  uint64_t counter = 0;
  uint64_t cycleLength = 0;
  platform = lastPlatform;
  std::map<platform_t, uint64_t> cache;
  bool checkCache = true;
  const auto TOTAL_SPINS =1000000000;
  while (counter++ < TOTAL_SPINS) {
    spinCycle(platform);
    auto score = scorePlatform(platform);
    if (checkCache && cache.contains(platform)) {
      cycleLength = counter - cache.at(platform);
      // fmt::println("Found cycle of length {} at counter={}", cycleLength, counter);
      auto skipCycleCount = (TOTAL_SPINS - counter) / cycleLength;
      counter += skipCycleCount * cycleLength;
      checkCache = false;
    } else {
      cache.insert({platform, counter});
    }
    // fmt::println("{} -> score={}", counter, scorePlatform(platform));
  }
  uint64_t p2 = scorePlatform(platform);

  fmt::println("Day14: Part 1: {}", p1);
  fmt::println("Day14: Part 2: {}", p2);
}
