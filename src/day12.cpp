#include "utils.hpp"

#include <algorithm>
#include <tuple>
#include <deque>
#include <fmt/format.h>
#include <numeric>
#include <vector>
#include <string_view>
#include <map>

enum class condition {
  operational, // .
  damaged,     // #
  unknown,     // ?
};

condition parseCond(char c) {
  switch (c) {
    case '.': return condition::operational;
    case '#': return condition::damaged;
    case '?': return condition::unknown;
  }
  __builtin_unreachable();
}

struct raw_line {
  std::deque<condition> row;
  std::deque<int> damaged_runs;
  bool operator==(const raw_line&) const = default;
  auto operator<=>(const raw_line&) const = default;
};

raw_line parseRawLine(std::string_view line) {
  raw_line res;

  while (line.front() != ' ') {
    auto curr = line.front();
    res.row.push_back(parseCond(curr));
    line.remove_prefix(1);
  }

  utils::eatChar(' ', line);
  while (!line.empty()) {
    res.damaged_runs.push_back(utils::parseInt(line));
    utils::eatLiteral(",", line);
  }

  return res;
}

std::string fmt_row(const std::deque<condition>& row) {
  std::string result;
  for (const auto c : row) {
    if (c == condition::damaged) result.push_back('#');
    else if (c == condition::operational) result.push_back('.');
    else if (c == condition::unknown) result.push_back('?');
  }

  return result;
}

template<bool Debug = false>
uint64_t calculatePossibilities(std::map<raw_line, uint64_t>& cache, raw_line inp) {
  auto [cache_it, unseen]  = cache.emplace(inp, 0);
  auto& result = cache_it->second;
  if constexpr (Debug) fmt::println("calculatePossibilities: {} - {} = {}", fmt_row(inp.row), fmt::join(inp.damaged_runs, ","), result);
  if (!unseen) return cache_it->second;

  if (inp.row.empty() && !inp.damaged_runs.empty()) { result = 0; return result; }
  if (inp.damaged_runs.empty()) {
    if (std::any_of(inp.row.begin(), inp.row.end(), [](const condition c) { return c == condition::damaged; }))
        result = 0;
    else result = 1;
    return result;
  }

  if (inp.row.front() == condition::operational) {
    while(!inp.row.empty() && inp.row.front() == condition::operational) { inp.row.pop_front(); }
    result = calculatePossibilities<Debug>(cache, inp);
  } else if (inp.row.front() == condition::damaged) {
    auto damageCount = inp.damaged_runs.front();
    inp.damaged_runs.pop_front();
    result = 1;
    while (damageCount-- > 0) {
      if (inp.row.empty()) { result = 0; break; }
      if (inp.row.front() == condition::operational) { result = 0; break; }
      inp.row.pop_front();
    }
    if (!inp.row.empty() && inp.row.front() == condition::damaged) { result = 0; } // next is damaged - wrong
    if (!inp.row.empty() && inp.row.front() == condition::unknown) { inp.row.front() = condition::operational; } // force next choice to be operational
    if (result) {
      result = calculatePossibilities<Debug>(cache, inp);
    }
  } else { // unknown!
    auto op_inp = inp;
    op_inp.row.pop_front();
    result = calculatePossibilities<Debug>(cache, op_inp); // if we say it's operational
    // otherwise it's damaged
    inp.row.front() = condition::damaged;
    result += calculatePossibilities<Debug>(cache, inp);
  }

  return result;
}

raw_line part2ize(raw_line& inp) {
  raw_line res = inp;

  for (int i = 0; i < 4; ++i) {
    res.row.push_back(condition::unknown);
    res.row.insert(res.row.end(), inp.row.begin(), inp.row.end());

    res.damaged_runs.insert(res.damaged_runs.end(), inp.damaged_runs.begin(), inp.damaged_runs.end());
  }
  return res;
}


void test() {
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day12_test.txt"};
  utils::LineReader lr{"inp/day12.txt"};

  uint64_t p1 = 0;
  uint64_t p2 = 0;
  std::map<raw_line, uint64_t> cache;
  while (auto line = lr.getLine()) {
    cache.clear();
    auto raw_line = parseRawLine(*line);
    auto res = calculatePossibilities(cache, raw_line);
    p1 += res;

    raw_line = part2ize(raw_line);
    res = calculatePossibilities(cache, raw_line);
    p2 += res;

    // fmt::println("{} -> {}", *line, res);
  }
  fmt::println("Day12: Part 1: {}", p1);
  fmt::println("Zay11: Part 2: {}", p2);
}
