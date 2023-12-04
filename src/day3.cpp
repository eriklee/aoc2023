#include <fstream>
#include <fmt/core.h>
#include <fmt/color.h>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <cassert>

#include "utils.hpp"

using schematic = std::vector<std::string>;
using gear_map = std::unordered_map<int, std::vector<uint64_t>>;

template<typename P>
bool check_perimeter(const schematic& map, int row, int n_start_col, int n_len, P pred) {
  auto start_col = n_start_col == 0 ? 0 : n_start_col - 1;
  auto end_col = n_start_col + n_len;
  if (end_col == map[0].size()) --end_col;

  if (row > 0) {
    --row;
    const std::string& line = map[row];
    for (int c = start_col; c <= end_col; ++c) {
      if (pred(line[c]))
        return true;
    }
    ++row;
  }
  if (pred(map[row][start_col])) {
    return true;
  }
  if (pred(map[row][end_col])) {
    return true;
  }

  ++row;
  if (row < map.size()) {
    const std::string& line = map[row];
    for (int c = start_col; c <= end_col; ++c) {
      if (pred(line[c]))
        return true;
    }
  }
  return false;
}

bool add_gear_part(gear_map& gm, int idx, uint64_t n) {
  auto gm_it = gm.try_emplace(idx);
  gm_it.first->second.push_back(n);
  return true;
}

bool update_gear_map(const schematic& map, gear_map& gm, int n, int row, int n_start_col, int n_len) {
  bool ret = false;
  auto start_col = n_start_col == 0 ? 0 : n_start_col - 1;
  auto end_col = n_start_col + n_len;
  if (end_col == map[0].size()) --end_col;
  const auto idx = [&map](auto r, auto c) { return (r * map[0].size() + c); };

  if (row > 0) {
    const std::string& line = map[row-1];
    for (int c = start_col; c <= end_col; ++c) {
      if (line[c] == '*') {
        ret = add_gear_part(gm, idx(row-1, c), n);
      }
    }
  }
  if (map[row][start_col] == '*')
    ret = add_gear_part(gm, idx(row, start_col), n);
  if (map[row][end_col] == '*')
    ret = add_gear_part(gm, idx(row, end_col), n);

  if (row + 1 < map.size()) {
    const std::string& line = map[row+1];
    for (int c = start_col; c <= end_col; ++c) {
      if (line[c] == '*')
        ret = add_gear_part(gm, idx(row+1, c), n);
    }
  }
  return ret;
}

template<bool debug, typename F, typename G>
void iterate(const schematic& map, F onNumber, G otherwise) {
  constexpr auto is_gear_part = [](auto c) { return c == '*'; };
  for (int r = 0; r < map.size(); ++r) {
    const std::string line = map[r];
    for (int c = 0; c < line.size(); ) {
      std::string_view lv = {line.begin() + c, line.end()};
      if (std::isdigit(lv.front())) {
        auto original_sz = lv.size();
        auto n = utils::parseInt(lv);
        auto n_len = original_sz - lv.size();
        onNumber(r, c, n, n_len);
        c += n_len;
      } else {
        otherwise(line[c], r * line.size() + c);
        ++c; }
    }
    if constexpr (debug) fmt::println("");
  }
}

template<bool debug = false>
uint64_t part1_iterate(const schematic& map) {
  uint64_t ret = 0;
  auto is_part_number_pred = [](const auto c) { return (c != '.' && !std::isdigit(c)); };
  auto onNumber = [&map, &ret, &is_part_number_pred](auto row, auto col, auto n, auto n_len) {
        auto ipn = check_perimeter(map, row, col, n_len, is_part_number_pred);
        if constexpr (debug) {
          if (ipn) fmt::print(fg(fmt::color::green), "{}", n);
          else fmt::print(fg(fmt::color::dark_red), "{}", n);
        }
        ret += ipn ? n : 0;
  };
  auto onSymbol = [](auto s, auto _idx){
        if constexpr(debug) {
          if (s == '.')
            fmt::print(fg(fmt::color::gray), "{}", s);
          else
            fmt::print(fmt::emphasis::bold, "{}", s);
        }
  };
  iterate<debug>(map, onNumber, onSymbol);

  return ret;
}

void dbg_print2_iter(const schematic& map, const gear_map& gm) {
  constexpr auto is_gear_part = [](auto c) { return c == '*'; };
  auto onNumber = [&map, is_gear_part](auto row, auto col, auto n, auto n_len) {
        auto igp = check_perimeter(map, row, col, n_len, is_gear_part);
        auto color = igp ? fg(fmt::color::green) : fg(fmt::color::gray);
        fmt::print(color, "{}", n);
  };
  auto onSymbol = [&gm](auto s, auto idx) {
        auto color = fg(fmt::color::gray);
        if (s == '*') {
          if (gm.contains(idx) && gm.at(idx).size() == 2)
            color = fg(fmt::color::light_green);
          else
            color = fg(fmt::color::orange_red);
        }
        fmt::print(color, "{}", s);
  };
  iterate<true>(map, onNumber, onSymbol);
}

template<bool debug = false>
uint64_t part2_iter(const schematic& map) {
  gear_map gm;
  auto onNumber = [&map, &gm](auto row, auto col, auto n, auto n_len) {
        auto igp = update_gear_map(map, gm, n, row, col, n_len);
        if constexpr (debug) {
          auto color = igp ? fg(fmt::color::green) : fg(fmt::color::gray);
          fmt::print(color, "{}", n);
        }
  };
  auto onSymbol = [&gm](auto s, auto idx) {
        if constexpr (debug) {
          auto color = s == '*' ? fg(fmt::color::light_green) : fg(fmt::color::gray);
          fmt::print(color, "{}", s);
        }
  };
  iterate<debug>(map, onNumber, onSymbol);
  if constexpr(debug) dbg_print2_iter(map, gm);

  uint64_t ret = 0;
  for (const auto [_k,vec] : gm) {
    if (vec.size() == 2) ret += vec[0] * vec[1];
  }
  return ret;
}

void test() {
  auto is_part_number_pred = [](const auto c) { return (c != '.' && !std::isdigit(c)); };
  {
    schematic m = {{"...440...."},
                   {"........#."}};
    utils::AssertEq(check_perimeter(m, 0, 3, 3, is_part_number_pred), false);
  }
  {
    schematic m = {{"...440#..."},
                   {"........#."}};
    utils::AssertEq(check_perimeter(m, 0, 3, 3, is_part_number_pred), true);
  }
  {
    schematic m = {{"...440...."},
                   {"......#.#."}};
    utils::AssertEq(check_perimeter(m, 0, 3, 3, is_part_number_pred), true);
  }
  {
    gear_map gm;
    schematic m = {{"...209*418.."},
                   {"............"}};
    utils::AssertEq(update_gear_map(m, gm, 209, 0, 3, 3), true);
    utils::AssertEq(update_gear_map(m, gm, 418, 0, 7, 3), true);
    utils::AssertEq(gm.size(), 1ul);
  }
}

int main(int argc, char **argv) {

  test();

  schematic map;
  std::string l;
  // std::ifstream file("inp/day3_test.txt");
  std::ifstream file("inp/day3.txt");
  while (std::getline(file, l)) {
    map.push_back(std::move(l));
  }
  auto p1res = part1_iterate<false>(map);
  auto p2res = part2_iter<false>(map);
  fmt::println("Day3: Part 1: {}", p1res);
  fmt::println("Day3: Part 2: {}", p2res);
}
