#include "utils.hpp"

#include <fstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <deque>
#include <bitset>

int cardMatchCount(std::string_view line) {
  utils::eatLiteral("Card ", line);
  utils::eatSpaces(line);
  auto gameId = utils::parseInt(line);
  utils::eatLiteral(": ", line);

  std::bitset<128> winners;
  while (line.front() != '|') {
    if (line.front() == ' ') utils::eatSpaces(line);
    else winners.set(utils::parseInt(line));
  }
  utils::eatChar('|', line);
  uint64_t result = 0;
  while (!line.empty()) {
    if (line.front() == ' ') utils::eatSpaces(line);
    else if (winners.test(utils::parseInt(line))) result = ++result;
  }

  return result;
}

int cardValue(int matchCount) {
  return (1 << matchCount) >> 1;
}

void extendCounts(std::deque<uint64_t>& counts, int cardCount, int matches) {
  counts.push_back(1); // always add at least one card
  while (counts.size() < matches) counts.push_back(1);
  for (int x = 0; x < matches; ++x) counts[x] += cardCount;
}

void test() {
  std::string l = "Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53";
  utils::AssertEq(cardValue(cardMatchCount(l)), 8);
  std::deque<uint64_t> counts;
  extendCounts(counts, 1, 4);
  utils::AssertEq(counts.size(), 4ul);
  for (const auto v : counts) utils::AssertEq(v, 2ul);
}

int main(int argc, char **argv) {

  // test();
  std::string l;
  {
    // std::ifstream file("inp/day4_test.txt");
    std::ifstream file("inp/day4.txt");
    uint64_t p1 = 0;
    uint64_t p2 = 0;
    std::deque<uint64_t> counts{1};
    while (std::getline(file, l)) {
      auto matches = cardMatchCount(l);
      p1 += cardValue(matches);
      auto copies = counts.front();
      p2 += copies;
      counts.pop_front();
      extendCounts(counts, copies, matches);
    }
    fmt::println("Day4: Part 1: {}", p1);
    fmt::println("Day4: Part 2: {}", p2);
  }
}
