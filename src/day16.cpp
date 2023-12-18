#include "utils.hpp"

#include <algorithm>
#include <fmt/format.h>
#include <vector>
#include <string_view>

using contraption_t = std::vector<std::string_view>;
using lightfield_t = std::vector<std::vector<uint8_t>>;

enum class Direction : uint8_t {
  Up  = 1,
  Right = 2,
  Down = 4,
  Left = 8,
};

struct beam_t {
  uint8_t row;
  uint8_t col;
  Direction dir;
};

void initializeLightfield(lightfield_t& lightfield, const contraption_t& contraption) {
  for (const auto line : contraption) {
    lightfield.emplace_back(line.size(), 0);
  }
}
void clearLightfield(lightfield_t& lightfield) {
  for (auto& line : lightfield) {
    for (auto& c : line) {
      c = 0;
    }
  }
}

[[nodiscard]]
bool visit(lightfield_t& lightfield, const beam_t beam) {
  bool alreadyVisited = lightfield[beam.row][beam.col] & static_cast<std::underlying_type<Direction>::type>(beam.dir);
  lightfield[beam.row][beam.col] |= static_cast<std::underlying_type<Direction>::type>(beam.dir);
  return alreadyVisited;
}

void moveBeam(beam_t& beam) {
  using enum Direction;
  if (beam.dir == Up) --beam.row;
  if (beam.dir == Down) ++beam.row;
  if (beam.dir == Left) --beam.col;
  if (beam.dir == Right) ++beam.col;
}

std::vector<beam_t> interactBeam(beam_t beam, char tile) {
  using enum Direction;
  if (tile == '.') {
    moveBeam(beam);
    return {beam};
  } else if (tile == '/') {
    switch (beam.dir) {
      case Up: beam.dir = Right; break;
      case Right: beam.dir = Up; break;
      case Down: beam.dir = Left; break;
      case Left: beam.dir = Down; break;
    }
    moveBeam(beam);
    return {beam};
  } else if (tile == '\\') {
    switch (beam.dir) {
      case Up: beam.dir = Left; break;
      case Right: beam.dir = Down; break;
      case Down: beam.dir = Right; break;
      case Left: beam.dir = Up; break;
    }
    moveBeam(beam);
    return {beam};
  } else if (tile == '-') {
    if (beam.dir == Right || beam.dir == Left) {
      moveBeam(beam);
      return {beam};
    } else {
      auto beamLeft = beam;
      beamLeft.dir = Left;
      moveBeam(beamLeft);
      auto beamRight = beam;
      beamRight.dir = Right;
      moveBeam(beamRight);
      return {beamLeft, beamRight};
    }
  } else if (tile == '|') {
    if (beam.dir == Up || beam.dir == Down) {
      moveBeam(beam);
      return {beam};
    } else {
      auto beamUp = beam;
      beamUp.dir = Up;
      moveBeam(beamUp);
      auto beamDown = beam;
      beamDown.dir = Down;
      moveBeam(beamDown);
      return {beamUp, beamDown};
    }
  }
  __builtin_unreachable();
}

std::vector<beam_t> stepBeam(const contraption_t& contraption, lightfield_t& lightfield, beam_t beam) {
  // Only need to check one direction because beam locations are unsigned
  if (beam.row >= lightfield.size()) return {};
  if (beam.col >= lightfield[beam.row].size()) return {};
  // Don't repeat visits we've made
  if (visit(lightfield, beam)) return {};

  const char tile = contraption[beam.row][beam.col];

  return interactBeam(beam, tile);
}

void runBeam(const contraption_t& contraption, lightfield_t& lightfield, beam_t initialB) {
  std::vector<beam_t> current = {initialB};
  std::vector<beam_t> next;
  while (!current.empty()) {
    while (!current.empty()) {
      auto b = current.back();
      current.pop_back();
      auto result = stepBeam(contraption, lightfield, b);
      next.insert(next.end(), result.begin(), result.end());
    }
    current.swap(next);
  }
}

template<bool Debug>
uint64_t countEnergizes(const lightfield_t& lightfield) {
  uint64_t result = 0;
  for (const auto& row : lightfield) {
    for (const auto c : row) {
      if constexpr (Debug) fmt::print("{}", c > 0 ? '#' : '.');
      result +=  c > 0;
    }
    if constexpr (Debug) fmt::println(" r:{}", result);
  }
  if constexpr (Debug) fmt::println(" r:{}", result);
  return result;
}

template<bool Debug = false>
uint64_t checkAllEntrances(const contraption_t& contraption, lightfield_t& lightfield) {
  uint64_t result = 0;

  for (int r = 0; r < contraption.size(); ++r) {
    clearLightfield(lightfield);
    if constexpr (Debug) fmt::println("Checking: {},{} d:{}", r,0,"right");
    runBeam(contraption, lightfield, beam_t{static_cast<uint8_t>(r), 0, Direction::Right});
    result = std::max(result, countEnergizes<Debug>(lightfield));

    clearLightfield(lightfield);
    if constexpr (Debug) fmt::println("Checking: {},{} d:{}", r,contraption[r].size() - 1, "left");
    runBeam(contraption, lightfield, beam_t{static_cast<uint8_t>(r), static_cast<uint8_t>(contraption[r].size() - 1), Direction::Left});
    result = std::max(result, countEnergizes<Debug>(lightfield));
  }

  for (int c = 0; c < contraption[0].size(); ++c) {
    clearLightfield(lightfield);
    if constexpr (Debug) fmt::println("Checking: {},{} d:{}", 0, c, "down");
    runBeam(contraption, lightfield, beam_t{0, static_cast<uint8_t>(c), Direction::Down});
    result = std::max(result, countEnergizes<Debug>(lightfield));

    clearLightfield(lightfield);
    if constexpr (Debug) fmt::println("Checking: {},{} d:{}", contraption.size() - 1, c, "up");
    runBeam(contraption, lightfield, beam_t{static_cast<uint8_t>(contraption.size() - 1), static_cast<uint8_t>(c), Direction::Up});
    result = std::max(result, countEnergizes<Debug>(lightfield));
  }
  return result;
}

void test() {
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day16_test.txt"};
  utils::LineReader lr{"inp/day16.txt"};

  contraption_t contraption;
  while (auto line = lr.getLine()) {
      contraption.push_back(*line);
  }
  lightfield_t lightfield;
  initializeLightfield(lightfield, contraption);

  runBeam(contraption, lightfield, beam_t{0,0,Direction::Right});
  auto p1 = countEnergizes<false>(lightfield);

  auto p2 = checkAllEntrances(contraption, lightfield);


  fmt::println("Day16: Part 1: {}", p1);
  fmt::println("Day16: Part 2: {}", p2);
}
