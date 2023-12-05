#include <cassert>
#include <fstream>
#include <fmt/core.h>
#include <string>
#include <string_view>

#include "utils.hpp"

struct rgb {
  int r = 0;
  int g = 0;
  int b = 0;

  int power() { return r * g * b; }
};

rgb combine(const rgb& l, const rgb&r) {
  rgb ret;
  ret.r = std::max(l.r, r.r);
  ret.g = std::max(l.g, r.g);
  ret.b = std::max(l.b, r.b);
  return ret;
}

bool possible(const rgb& bs) {
  return bs.r <= 12 && bs.g <= 13 && bs.b <= 14;
}

struct pp {
  int possible_id;
  int power;

  pp& operator+=(const pp& rhs) {
    this->possible_id += rhs.possible_id;
    this->power += rhs.power;
    return *this;
  }
};

pp part1And2(std::string_view line) {
  utils::eatLiteral("Game ", line);
  int gameId = utils::parseInt(line);
  utils::eatLiteral(": ", line);
  // fmt::print("Game {}:", gameId);
  rgb minballs;
  while (!line.empty()) {
    rgb ballset;
    while (!line.empty() && line.front() != ';') {
      auto count = utils::parseInt(line);
      utils::eatChar(' ', line);


      if (utils::eatLiteral("red", line)) ballset.r = count;
      else if (utils::eatLiteral("green", line)) ballset.g = count;
      else if (utils::eatLiteral("blue", line)) ballset.b = count;
      else __builtin_unreachable();

      utils::eatLiteral(", ", line);
    }
    utils::eatLiteral("; ", line);
    // fmt::print(" {} red, {} green, {} blue;", ballset.r, ballset.g, ballset.b);
    minballs = combine(minballs, ballset);
  }
  // fmt::println(" -> {}r {}g {}b", minballs.r, minballs.g, minballs.b);
  return {possible(minballs) ? gameId : 0, minballs.power()};
}

void test() {
  rgb t1{4,0,3};
  rgb t2{1,2,6};
  const auto res = combine(t1, t2);
  assert(res.r == 4);
  assert(res.g == 2);
  assert(res.b == 6);
}

int main(int argc, char **argv) {

  // test();

  std::string l;
  // std::ifstream file("inp/day2_test.txt");
  std::ifstream file("inp/day2.txt");
  pp res{0, 0};
  while (std::getline(file, l)) {
    res += part1And2(l);
  }
  fmt::println("Day2: Part 1: {}", res.possible_id);
  fmt::println("Day2: Part 2: {}", res.power);
}
