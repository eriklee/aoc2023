#include "utils.hpp"

#include <tuple>
#include <fmt/format.h>
#include <fmt/color.h>
#include <numeric>
#include <vector>
#include <string_view>
#include <set>

using diagram_t = std::vector<std::string_view>;
using coord_t = std::pair<int, int>;

coord_t findStartLocation(const diagram_t& d) {
  coord_t result;
  for (int r = 0; r < d.size(); ++r) {
    for (int c = 0; c < d[r].size(); ++c) {
      if (d[r][c] == 'S') return {r, c};
    }
  }
  __builtin_unreachable();
}

coord_t nextStep(const char curr_tile, coord_t curr, coord_t prev) {
  if (curr_tile == '|') { 
    if (prev.first < curr.first) return { curr.first + 1, curr.second };
    else return { curr.first - 1, curr.second };
  } else if (curr_tile == '-') { 
    if (prev.second < curr.second) return { curr.first, curr.second + 1 };
    else return { curr.first, curr.second - 1 };
  } else if (curr_tile == 'L') { 
    if (prev.first < curr.first) // curr is below prev
      return { curr.first, curr.second + 1 };
    else return { curr.first - 1, curr.second };
  } else if (curr_tile == 'J') { 
    if (prev.first < curr.first) // curr is below prev
      return { curr.first, curr.second - 1 };
    else return { curr.first - 1, curr.second };
  } else if (curr_tile == '7') { 
    if (prev.first > curr.first) // curr is above prev
      return { curr.first, curr.second - 1 };
    else return { curr.first + 1, curr.second };
  } else if (curr_tile == 'F') { 
    if (prev.first > curr.first) // curr is above prev
      return { curr.first, curr.second + 1 };
    else return { curr.first + 1, curr.second };
  }
  __builtin_unreachable();
}

bool westCompatible(char tile) {
  return (tile == '-' | tile == 'F' | tile == 'L');
}
bool eastCompatible(char tile) {
  return (tile == '-' | tile == 'J' | tile == '7');
}
bool northCompatible(char tile) {
  return (tile == '|' | tile == 'F' | tile == '7');
}
bool southCompatible(char tile) {
  return (tile == '|' | tile == 'J' | tile == 'L');
}

std::pair<coord_t, coord_t> findFirstSteps(const diagram_t& diagram, coord_t start) {
  coord_t fst = start;
  coord_t snd = start;
  if (start.first > 0 && northCompatible(diagram[start.first - 1][start.second])) {
    fst = { start.first - 1, start.second }; 
  }
  if (start.first < (diagram.size() - 1) && southCompatible(diagram[start.first + 1][start.second])) {
    auto& tgt = (fst == start) ? fst : snd;
    tgt = { start.first + 1, start.second }; 
  }
  if (start.second > 0 && westCompatible(diagram[start.first][start.second - 1])) {
    auto& tgt = (fst == start) ? fst : snd;
    tgt = { start.first, start.second - 1 }; 
  }
  if (start.second < (diagram[0].size() - 1) && eastCompatible(diagram[start.first][start.second + 1])) {
    auto& tgt = (fst == start) ? fst : snd;
    tgt = { start.first, start.second + 1 }; 
  }
  return {fst, snd};
}

template<bool Debug>
int part1Search(const diagram_t& diagram, coord_t explorer1, coord_t explorer1_prev, coord_t explorer2, coord_t explorer2_prev, std::vector<coord_t>& dbgPath) {
  int stepCount = 1;

  if constexpr (Debug) dbgPath.push_back(explorer1);
  if constexpr (Debug) dbgPath.push_back(explorer2);
  while (explorer1 != explorer2) {
    auto next1 = nextStep(diagram[explorer1.first][explorer1.second], explorer1, explorer1_prev);
    auto next2 = nextStep(diagram[explorer2.first][explorer2.second], explorer2, explorer2_prev);
    explorer1_prev = explorer1;
    explorer1 = next1;
    explorer2_prev = explorer2;
    explorer2 = next2;
    if constexpr (Debug) dbgPath.push_back(explorer1);
    if constexpr (Debug) dbgPath.push_back(explorer2);
    ++stepCount;
  }

  return stepCount;
}

void printDbgPath(const diagram_t& diagram, const std::vector<coord_t>& path) {
  std::set<coord_t> pathMap;
  for (const auto c : path) pathMap.insert(c);

  for (int r = 0; r < diagram.size(); ++r) {
    const auto& row = diagram[r];
    for (int c = 0; c < row.size(); ++c) {
      auto color = fmt::color::gray;
      if (row[c] == 'S') color = fmt::color::white;
      if (pathMap.contains({r,c})) color = fmt::color::light_green;
      if (path.back() == coord_t{r,c}) color = fmt::color::red;
      fmt::print(fg(color), "{}", row[c]);
    }
    fmt::println("");
  }
}

template<bool Debug>
int64_t part1(const diagram_t& diagram) {
  auto start = findStartLocation(diagram);
  auto [e1, e2] = findFirstSteps(diagram, start);
  if constexpr (Debug) fmt::println("S: ({},{}), searching beginning from ({},{}) and ({},{})", start.first, start.second, e1.first, e1.second, e2.first, e2.second);
  std::vector<coord_t> dbgPath;
  auto result = part1Search<Debug>(diagram, e1, start, e2, start, dbgPath);
  if constexpr (Debug) printDbgPath(diagram, dbgPath);
  return result;
}

void test() {
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day10_1.txt"};
  // utils::LineReader lr{"inp/day10_2.txt"};
  utils::LineReader lr{"inp/day10.txt"};
  diagram_t diagram;

  while (auto line = lr.getLine()) {
    diagram.push_back(*line);
  }
  auto p1 = part1<true>(diagram);
  fmt::println("Day10: Part 1: {}", p1);
  // auto p2 = part2<false>(instructions, graph);
  // fmt::println("Day9: Part 2: {}", p2);
}
