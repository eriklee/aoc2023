#include "utils.hpp"

#include <algorithm>
#include <tuple>
#include <fmt/format.h>
#include <fmt/color.h>
#include <numeric>
#include <vector>
#include <string_view>
#include <set>

enum class condition {
  operational, // .
  damaged,     // #
  unknown,     // ?
};

struct rle_elem {
  condition value;
  int length;
};

struct rle_line {
  std::vector<rle_elem> line;
  std::vector<int> damaged_runs;
};

condition parseCond(char c) {
  switch (c) {
    case '.': return condition::operational;
    case '#': return condition::damaged;
    case '?': return condition::unknown;
  }
  __builtin_unreachable();
}

rle_line parseLine(std::string_view line) {
  rle_line result;

  char last = line.front();
  int run_length = 0;
  while (line.front() != ' ') {
    auto curr = line.front();
    if (curr == last) {
      run_length++;
    } else {
      result.line.emplace_back(rle_elem{.value = parseCond(last), .length = run_length});
      last = curr;
      run_length = 1;
    }
    line.remove_prefix(1);
  }
  result.line.emplace_back(rle_elem{.value = parseCond(last), .length = run_length});
  

  utils::eatChar(' ', line);
  while (!line.empty()) {
    result.damaged_runs.push_back(utils::parseInt(line));
    utils::eatLiteral(",", line);
  }

  return result;
}

bool validLine(const rle_line& l) {
  auto l_it = l.line.begin();
  auto d_it = l.damaged_runs.begin();
  while (l_it != l.line.end() && d_it != l.damaged_runs.end()) {
    if (l_it->value == condition::damaged) {
      if (l_it->length != *d_it) return false;
      ++d_it; // it was fine, just increment everything
    }
    ++l_it;
  }
  while (l_it != l.line.end()) {
    auto cond = (l_it++)->value;

    if (cond == condition::damaged) return false;
  }
  return (l_it == l.line.end() && d_it == l.damaged_runs.end());
}

void canonicalizeLine(std::vector<rle_elem>& l) {
  auto l_it = l.begin();
  while (l_it != l.end()) {
    if (auto next_it = l_it + 1; next_it != l.end()) {
      if (l_it->value == next_it->value) {
        next_it->length += l_it->length;
        l_it = l.erase(l_it);
      } else {
        ++l_it;
      }
    } else {
      ++l_it;
    }
  }
}


std::vector<rle_line> chooseUnknown(const rle_line& inp) {
  // if we have an operational run we can just drop it (return only one element)
  // if we have a damaged run we can see if it matches the next damaged value
  //  - if so, remove it, and ensure the next value (if it exists) is operational, either already
  //    or by changing one element of the next run of unknown results (return only one element)
  //  - if not, and we can't extend return nothing
  // if we have an unknown run there are 2 cases for what might follow, operational or damaged
  //   if the follower is operational we can either:
  //      fill in the next damaged value (if possible)
  //        potentially followed by a 1-run of operational if there's space * return value.first
  //      mark the cell as operational (either with a new run or extending the past) and iterate once * return value.second
  //   if the follower is damaged and we need the values to get to the next damaged value, we can return a 
  //   list of lines with 0-length operational values
  //   if the follower is damaged but we have enough we can return a list of valid damaged values that fit in with an operational follower
  return {};
}

std::pair<int, int> sumElems(const std::vector<rle_elem>& inp) {
  int usum = 0;
  int dsum = 0;
  for (const auto& elem : inp) {
    if (elem.value == condition::unknown) usum += elem.length;
    if (elem.value == condition::damaged) dsum += elem.length;
  }
  return {usum, dsum};
}

int sumDamaged(const std::vector<int>& inp) {
  return std::accumulate(inp.begin(), inp.end(), 0);
}

std::vector<uint64_t> filterUnknownsWithDamages(const int damageCount, const int unknownCount) {
  std::vector<uint64_t> result;
  for (uint x = 0; x < (1 << unknownCount); x++) {
    if (std::popcount(x) == damageCount) result.push_back(x);
  }
  return result;
}

std::vector<rle_elem> fillLine(const std::vector<rle_elem>& inp, int potentialFillings) {
  std::vector<rle_elem> res;
  for (auto e : inp) {
    if (e.value == condition::unknown) {
      while (e.length-- > 0) {
        if (potentialFillings & 1) {
          res.push_back(rle_elem{condition::damaged, 1});
        } else {
          res.push_back(rle_elem{condition::operational, 1});
        }
        potentialFillings >>= 1;
      }
    } else {
      res.push_back(e);
    }
  }
  canonicalizeLine(res);
  return res;
}

int bruteForce(rle_line inp) {
  auto [unknownCount, damagedCount] = sumElems(inp.line);
  auto requiredDamages = sumDamaged(inp.damaged_runs);
  auto potentialFillings = filterUnknownsWithDamages(requiredDamages - damagedCount, unknownCount);

  int result = 0;
  const auto originalLine = inp.line;
  for (auto pf : potentialFillings) {
    inp.line = fillLine(originalLine, pf);
    if (validLine(inp)) result++;
  }

  return result;
}

void test() {
  auto ex = parseLine("???.###.. 1,1,3");
  utils::Assert(ex.damaged_runs.size() == 3);
  utils::Assert(ex.damaged_runs[0] == 1);
  utils::Assert(ex.damaged_runs[1] == 1);
  utils::Assert(ex.damaged_runs[2] == 3);

  utils::Assert(ex.line.size() == 4);
  utils::Assert(ex.line[0].value == condition::unknown);
  utils::Assert(ex.line[0].length == 3);
  utils::Assert(ex.line[1].value == condition::operational);
  utils::Assert(ex.line[1].length == 1);
  utils::Assert(ex.line[2].value == condition::damaged);
  utils::Assert(ex.line[2].length == 3);
  utils::Assert(ex.line[3].value == condition::operational);
  utils::Assert(ex.line[3].length == 2);

  utils::Assert(validLine(parseLine("#.#.### 1,1,3")));
  utils::Assert(validLine(parseLine(".#.....#....###. 1,1,3")));
  utils::Assert(validLine(parseLine("#.###.#.###### 1,3,1,6")));
  utils::Assert(validLine(parseLine("###..##.....# 3,2,1")));
  utils::Assert(!validLine(parseLine("###..##..#..# 3,2,1")));
  utils::Assert(!validLine(parseLine("###..##..#..# 4,2,1")));
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day12_test.txt"};
  utils::LineReader lr{"inp/day12.txt"};

  uint64_t p1 = 0;
  while (auto line = lr.getLine()) {
    auto rle_line = parseLine(*line);
    auto res = bruteForce(rle_line);
    p1 += res;
    fmt::println("{} -> {}", *line, res);
  }
  fmt::println("Day12: Part 1: {}", p1);
  // fmt::println("Day11: Part 2: {}", p2);
}
