#include "utils.hpp"

#include <algorithm>
#include <fmt/format.h>
#include <vector>
#include <string_view>

using history_t = std::vector<int64_t>;

history_t parseHistory(std::string_view line) {
  history_t result;
  while (!line.empty()) {
    result.push_back(utils::parseInt(line));
    utils::eatSpaces(line);
  }
  return result;
}

template<bool Debug>
int64_t part1(history_t hist) {
  // We can repeatedly apply the diff to the history, and accumulate the last entry in
  // the (shortening) list every time into the result;
  int64_t result = hist.back();
  if constexpr (Debug) fmt::print("{} = {}", fmt::join(hist, ", "), result);
  int place = hist.size() - 1;
  while (place > 0) {
    bool anyNonZero = false;
    for (int i = 0; i < place; ++i) {
      hist[i] = hist[i + 1] - hist[i]; // The difference at each step - is this the absolute difference?? No
      anyNonZero |= hist[i];
    }
    if constexpr (Debug) fmt::print(" + {}", hist[place]);
    result += hist[--place];
    if (!anyNonZero) break;
  }
  if constexpr (Debug) fmt::println(" = {}", result);
  return result;
}

void test() {
  utils::Assert(parseHistory("0 3 6 9 12 15").size() == 6);
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day9_test.txt"};
  utils::LineReader lr{"inp/day9.txt"};

  int64_t p1 = 0;
  int64_t p2 = 0;
  while (auto line = lr.getLine()) {
    auto history = parseHistory(*line);
    p1 += part1<false>(history);
    std::reverse(history.begin(), history.end());
    p2 += part1<false>(history);
  }
  fmt::println("Day9: Part 1: {}", p1);
  fmt::println("Day9: Part 2: {}", p2);
}
