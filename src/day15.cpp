#include "utils.hpp"

#include <algorithm>
#include <tuple>
#include <deque>
#include <fmt/format.h>
#include <numeric>
#include <vector>
#include <string_view>
#include <map>

uint8_t calculateHASH(std::string_view sv) {
  uint8_t result = 0;
  for (const char c : sv) {
    result += c;
    result *= 17;
  }
  return result;
}

void test() {
  auto hashhash = calculateHASH("HASH");
  utils::AssertEq(static_cast<int>(hashhash), 52);
}

std::optional<std::string_view> getSequence(std::string_view& line) {
  if (line.empty()) return {};

  auto breakidx = line.find(',');
  std::string_view result = line.substr(0,breakidx);
  line.remove_prefix(std::min(line.size(), result.size() + 1));
  return result;
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day15_test.txt"};
  utils::LineReader lr{"inp/day15.txt"};

  uint64_t p1 = 0;
  auto line = lr.getLine();
  while (auto step = getSequence(*line)) {
    p1 += calculateHASH(*step);
    fmt::println("{} becomes {}.", *step, calculateHASH(*step));
  }
      
  fmt::println("Day15: Part 1: {}", p1);
  // fmt::println("Day15: Part 2: {}", p2);
}
