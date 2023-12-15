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

void test() {
}

bool checkReflection(std::string_view line, int idx) {
  int left = idx + 1;
  int right = idx;
  bool reflection = true;

  while (--left >= 0 && ++right < line.size()) {
    reflection &= line[left] == line[right];
  }
  return reflection;
}

template<bool Debug = true>
uint64_t findReflection(const pattern_t& pattern) {
  if constexpr (Debug) { fmt::println("findReflection called:\n\t{}", fmt::join(pattern, "\n\t"));}
  // search for horizontal reflection
  for (int i = 0; i < pattern.size() - 1; ++i) {
    int top = i+1;
    int bottom = i;
    bool reflection = true;
    while (reflection && --top >= 0 && ++bottom < pattern.size()) {
      reflection &= pattern[bottom] == pattern[top];
    }
    if constexpr (Debug) if (reflection) fmt::println("findReflection: horizontal: i={}", i);
    if (reflection) return 100 * (i + 1);
  }

  const int width = pattern[0].size();
  // find a vertical line of reflection
  for (int i = 0; i < width - 1; ++i) {
    bool reflection = true;
    for (int r = 0; r < pattern.size(); ++r) {
      reflection &= checkReflection(pattern[r], i);
      if (!reflection) break;
    }
    if constexpr (Debug) if (reflection) fmt::println("findReflection: vertical: i={}", i);
    if (reflection) return i + 1;
  }

  return 0;
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
      p1 += findReflection(pat);
      pat.clear();
    } else {
      pat.push_back(*line);
    }
  }
  p1 += findReflection(pat);

  fmt::println("Day13: Part 1: {}", p1);
  fmt::println("Day13: Part 2: {}", p2);
}
