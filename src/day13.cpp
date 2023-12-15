#include "utils.hpp"

#include <algorithm>
#include <tuple>
#include <deque>
#include <fmt/format.h>
#include <numeric>
#include <vector>
#include <string_view>
#include <map>

using pattern_t = std::vector<std::string_view>;

// now return the distance between the two values
int checkReflection(std::string_view line, int idx) {
  int left = idx + 1;
  int right = idx;
  int reflection = 0;

  while (--left >= 0 && ++right < line.size()) {
    reflection += (line[left] != line[right]);
  }
  return reflection;
}

int distance(std::string_view l1, std::string_view l2) {
  int distance = 0;
  for (int i = 0; i < l1.size(); ++i) {
    distance += (l1[i] != l2[i]);
  }
  return distance;
}

template<int SmudgeFactor, bool Debug = true>
uint64_t findReflection(const pattern_t& pattern) {
  if constexpr (Debug) fmt::println("findReflection<{}> called:\n\t{}", SmudgeFactor, fmt::join(pattern, "\n\t"));
  // search for horizontal reflection
  for (int i = 0; i < pattern.size() - 1; ++i) {
    int top = i+1;
    int bottom = i;
    int smudges = 0;
    while (smudges <= SmudgeFactor && --top >= 0 && ++bottom < pattern.size()) {
      smudges += distance(pattern[bottom], pattern[top]);
    }
    if constexpr (Debug) if (smudges == SmudgeFactor) fmt::println("findReflection: horizontal: i={}", i);
    if (smudges == SmudgeFactor) return 100 * (i + 1);
  }

  const int width = pattern[0].size();
  // find a vertical line of reflection
  for (int i = 0; i < width - 1; ++i) {
    int smudges = 0;
    for (int r = 0; r < pattern.size(); ++r) {
      smudges += checkReflection(pattern[r], i);
      if (smudges > SmudgeFactor) break;
    }
    if constexpr (Debug) if (smudges == SmudgeFactor) fmt::println("findReflection: vertical: i={}", i);
    if (smudges == SmudgeFactor) return i + 1;
  }

  return 0;
}

void test() {
  utils::AssertEq(distance("##...##.", "##...##."), 0);
  utils::AssertEq(distance("##.#.##.", "##...##."), 1);
  utils::AssertEq(distance("##.#.##.", "#....##."), 2);
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day13_test.txt"};
  utils::LineReader lr{"inp/day13.txt"};

  uint64_t p1 = 0;
  uint64_t p2 = 0;
  pattern_t pat;
  while (auto line = lr.getLine()) {
    if (line->empty()) {
      p1 += findReflection<0>(pat);
      p2 += findReflection<1>(pat);
      pat.clear();
    } else {
      pat.push_back(*line);
    }
  }
  p1 += findReflection<0>(pat);
  p2 += findReflection<1>(pat);

  fmt::println("Day13: Part 1: {}", p1);
  fmt::println("Day13: Part 2: {}", p2);
}
