#include "utils.hpp"

#include <cmath>
#include <fmt/core.h>
#include <string>
#include <string_view>

int eval(int64_t raceTime, int64_t holdTime) {
  return holdTime * (raceTime - holdTime);
}

template<bool Debug = false>
int64_t winningTimes(int64_t raceTime, int64_t raceRecord) {
  // Calculating the part of the quadratic formula that gives the distance from the midpoint of the 2 roots
  double dist = std::sqrt((raceTime * raceTime) - (4 * raceRecord)) / 2;
  double midpoint = static_cast<double>(raceTime) / 2;
  int64_t minTime = std::ceil(midpoint - dist);
  int64_t maxTime = std::floor(midpoint + dist);

  // We need to *beat* the record time
  if (eval(raceTime, minTime) == raceRecord) minTime++;
  if (eval(raceTime, maxTime) == raceRecord) maxTime--;

  if constexpr (Debug) {
    fmt::println("wT(raceTime={}, raceRecord={}) -> dist={} win between {} and {}",
        raceTime, raceRecord, dist, minTime, maxTime);
  }
   return maxTime - minTime + 1;
}

void test() {
  utils::AssertEq(winningTimes(7, 9), 4l);
  utils::AssertEq(winningTimes(15, 40), 8l);
  utils::AssertEq(winningTimes(30, 200), 9l);
}

int main(int argc, char **argv) {
  // test();
  // utils::LineReader lr{"inp/day6_test.txt"};
  utils::LineReader lr{"inp/day6.txt"};
  auto timeline  = *lr.getLine(); utils::eatLiteral("Time:", timeline);
  auto distline  = *lr.getLine(); utils::eatLiteral("Distance:", distline);

  int64_t winWaysCountProduct = 1;
  std::string p2time, p2dist;

  while (!timeline.empty()) {
    utils::eatSpaces(timeline); utils::eatSpaces(distline);
    auto time = utils::parseInt(timeline);
    auto distance = utils::parseInt(distline);

    // This is so, so lazy, but I went for a very long bike ride and have dinner plans, sue me
    p2time += std::to_string(time);
    p2dist += std::to_string(distance);

    winWaysCountProduct *= winningTimes(time, distance);
  }

  fmt::println("Day6: Part 1: {}", winWaysCountProduct);
  fmt::println("Day6: Part 2: {}", winningTimes(std::stol(p2time), std::stol(p2dist)));
}
