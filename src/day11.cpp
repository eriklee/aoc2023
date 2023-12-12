#include "utils.hpp"

#include <algorithm>
#include <tuple>
#include <fmt/format.h>
#include <fmt/color.h>
#include <numeric>
#include <vector>
#include <string_view>
#include <set>

using diagram_t = std::vector<std::string_view>;
using coord_t = std::pair<int64_t, int64_t>;

std::vector<int64_t> findEmptyRows(const diagram_t& d) {
  std::vector<int64_t> result;
  for (int r = 0; r < d.size(); ++r) {
    if (std::all_of(d[r].begin(), d[r].end(),
          [](const auto pix) { return pix == '.'; }))
      result.push_back(r);
  }
  return result;
}

std::vector<int64_t> findEmptyCols(const diagram_t& d) {
  std::vector<int64_t> result;
  for (int c = 0; c < d[0].size(); ++c) {
    bool noGalaxies = true;
    for (int r = 0; r < d.size(); ++r) {
      noGalaxies &= d[r][c] == '.';
    }
    if (noGalaxies) result.push_back(c);
  }
  return result;
}

std::set<coord_t> buildGalacticMap(const diagram_t& d) {
  std::set<coord_t> result;
  for (int r = 0; r < d.size(); ++r) {
    for (int c = 0; c < d[r].size(); ++c) {
      if (d[r][c] == '#') result.insert({r, c});
    }
  }
  return result;
}

int64_t getExpandedCoord(int64_t initial, std::vector<int64_t> expansionIndices, const int64_t factor) {
  auto add = 0;
  for (const auto idx : expansionIndices) {
    if (initial < idx) break;
    else add += (factor - 1);
  }
  return initial + add;
}

std::set<coord_t> expandGalacticMap(const diagram_t& d, std::set<coord_t>& initial, const int64_t factor) {
  auto rowExpansions = findEmptyRows(d);
  auto colExpansions = findEmptyCols(d);
  std::set<coord_t> result;

  for (const auto [r,c] : initial) {
    auto expR = getExpandedCoord(r, rowExpansions, factor);
    auto expC = getExpandedCoord(c, colExpansions, factor);
    result.insert({expR, expC});
  }

  return result;
}

int64_t shortestPath(coord_t x, coord_t y) { return std::abs(x.first - y.first) + std::abs(x.second - y.second); }

int64_t getAllShortestPaths(std::set<coord_t>& galaxies) {
  std::vector<coord_t> flattened;
  for (const auto gal : galaxies) { flattened.push_back(gal); }

  int64_t result = 0;
  for (auto g1 = flattened.begin(); g1 != flattened.end(); ++g1) {
    auto g2 = g1;
    while (++g2 != flattened.end()) {
      result += shortestPath(*g1, *g2);
    }
  }
  return result;
}

void test() {
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day11_test.txt"};
  utils::LineReader lr{"inp/day11.txt"};
  diagram_t diagram;

  while (auto line = lr.getLine()) {
    diagram.push_back(*line);
  }
  auto gm = buildGalacticMap(diagram);
  auto gm_part1 = expandGalacticMap(diagram, gm, 2);
  auto p1 = getAllShortestPaths(gm_part1);
  fmt::println("Day11: Part 1: {}", p1);

  /*
  {
  auto gm_part2 = expandGalacticMap(diagram, gm, 10);
  auto p2 = getAllShortestPaths(gm_part2);
  fmt::println("Day11: Part 2: 10x={}", p2);
  }
  {
  auto gm_part2 = expandGalacticMap(diagram, gm, 100);
  auto p2 = getAllShortestPaths(gm_part2);
  fmt::println("Day11: Part 2: 100x={}", p2);
  }
  */
  auto gm_part2 = expandGalacticMap(diagram, gm, 1000000);
  auto p2 = getAllShortestPaths(gm_part2);
  fmt::println("Day11: Part 2: {}", p2);
}
