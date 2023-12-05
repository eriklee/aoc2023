#include "utils.hpp"

#include <benchmark/benchmark.h>

#include <fstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <deque>
#include <bitset>

int cardMatchCount_us(std::string_view line) {
  utils::eatLiteral("Card ", line);
  utils::eatSpaces(line);
  auto gameId = utils::parseInt(line);
  utils::eatLiteral(": ", line);

  std::unordered_set<uint8_t> winners;
  while (line.front() != '|') {
    if (line.front() == ' ') utils::eatSpaces(line);
    else winners.insert(utils::parseInt(line));
  }
  utils::eatChar('|', line);
  uint64_t result = 0;
  while (!line.empty()) {
    if (line.front() == ' ') utils::eatSpaces(line);
    else if (winners.contains(utils::parseInt(line))) result = ++result;
  }

  return result;
}
int cardMatchCount_bs(std::string_view line) {
  utils::eatLiteral("Card ", line);
  utils::eatSpaces(line);
  auto gameId = utils::parseInt(line);
  utils::eatLiteral(": ", line);

  std::bitset<128> winners;
  // std::unordered_set<uint8_t> winners;
  while (line.front() != '|') {
    if (line.front() == ' ') utils::eatSpaces(line);
    // else winners.insert(utils::parseInt(line));
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

static void BM_cmc_bs(benchmark::State& state) {

  std::string l;
  for (auto _ : state)
  {
    // std::ifstream file("inp/day4_test.txt");
    std::ifstream file("inp/day4.txt");
    uint64_t p1 = 0;
    while (std::getline(file, l)) {
      auto matches = cardMatchCount_bs(l);
      p1 += cardValue(matches);
    }
  }
}

static void BM_cmc_us(benchmark::State& state) {

  std::string l;
  for (auto _ : state)
  {
    // std::ifstream file("inp/day4_test.txt");
    std::ifstream file("inp/day4.txt");
    uint64_t p1 = 0;
    while (std::getline(file, l)) {
      auto matches = cardMatchCount_us(l);
      p1 += cardValue(matches);
    }
  }
}

static void BM_cmc_lr(benchmark::State& state) {

  for (auto _ : state)
  {
    // std::ifstream file("inp/day4_test.txt");
    auto lr = utils::LineReader("inp/day4.txt");
    uint64_t p1 = 0;
    while (auto l = lr.getLine()) {
      auto matches = cardMatchCount_bs(*l);
      p1 += cardValue(matches);
    }
  }
}
BENCHMARK(BM_cmc_us);
BENCHMARK(BM_cmc_bs);
BENCHMARK(BM_cmc_lr);
BENCHMARK_MAIN();
