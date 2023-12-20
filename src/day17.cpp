#include "utils.hpp"

#include <algorithm>
#include <fmt/format.h>
#include <vector>
#include <string_view>
#include <map>
#include <queue>

enum class Direction : uint8_t {
  Up  = 1,
  Right = 2,
  Down = 4,
  Left = 8,
};

struct state_t {
  int32_t heat_loss;
  uint8_t row;
  uint8_t col;
  Direction dir;
  uint8_t count;

  auto operator<=>(const state_t& state) const = default;
};

using map_t = std::vector<std::string_view>;
using cache_t = std::map<state_t, int32_t>; // state stored with hl=0

[[nodiscard]]
bool visit(cache_t& cache, state_t state) {
  int curr = state.heat_loss;
  state.heat_loss = 0;
  int& last = cache[state];
  auto alreadyVisited = last < 0 && curr <= last;
  if (!alreadyVisited) last = curr;
  return alreadyVisited;
}

void moveState(state_t& state) {
  using enum Direction;
  if (state.dir == Up) --state.row;
  if (state.dir == Down) ++state.row;
  if (state.dir == Left) --state.col;
  if (state.dir == Right) ++state.col;
  ++state.count;
}

std::vector<state_t> nextSteps(state_t state) {
  std::vector<state_t> result;
  result.reserve(4);
  if (state.count < 3) {
    result.push_back(state);
    moveState(result.back());
  }

  using enum Direction;
  result.push_back(state);
  result.back().count = 0;
  auto& a = result.back();
  result.push_back(state);
  result.back().count = 0;
  auto& b = result.back();

  if (state.dir == Up || state.dir == Down) {
    a.dir = Left;
    moveState(a);
    b.dir = Right;
    moveState(b);
  } else if (state.dir == Left || state.dir == Right) {
    a.dir = Up;
    moveState(a);
    b.dir = Down;
    moveState(b);
  }

  return result;
}

std::vector<state_t> stepState(const map_t& cityMap, cache_t& cache, state_t state) {
  if (visit(cache, state)) return {};

  auto next = nextSteps(state);
  for (auto it = next.begin(); it != next.end();) {
    if (it->row >= cityMap.size()) {
      it = next.erase(it);
      continue;
    }
    if (it->col >= cityMap[it->row].size()) {
      it = next.erase(it);
      continue;
    }

    const int tile = cityMap[it->row][it->col] - '0';
    it->heat_loss -= tile;
    ++it;
  }
  return next;
}
void addToQ(std::vector<state_t>& vec, std::priority_queue<state_t>& q) {
  for (const auto& x : vec) q.push(x);
  vec.clear();
}

int64_t runSearch(const map_t& map, cache_t& cache) {
  const auto initialLoss = 0;
  const auto goalRow = map.size() - 1;
  const auto goalCol = map.back().size() - 1;

  std::vector<state_t> next = {state_t{initialLoss,0,0,Direction::Right,0}, state_t{initialLoss,0,0,Direction::Down, 0}};
  std::priority_queue<state_t> q;
  addToQ(next, q);
  
  while (!q.empty()) {
    auto s = q.top();
    q.pop();
    next = stepState(map, cache, s);
    for (const auto& s : next) {
      if (s.row == goalRow && s.col == goalCol) return -(s.heat_loss);
    }
    addToQ(next, q);
  }
  return 0;
}

void test() {
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day17_test.txt"};
  utils::LineReader lr{"inp/day17.txt"};

  map_t cityMap;
  while (auto line = lr.getLine()) {
      cityMap.push_back(*line);
  }
  cache_t cache;

  auto p1 = runSearch(cityMap, cache);

  auto p2 = 0;


  fmt::println("Day16: Part 1: {}", p1);
  fmt::println("Day16: Part 2: {}", p2);
}
