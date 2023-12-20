#include "utils.hpp"

#include <algorithm>
#include <fmt/format.h>
#include <vector>
#include <string_view>

enum class Direction : uint8_t {
  Up  = 1,
  Right = 2,
  Down = 4,
  Left = 8,
};

struct tile_t {
 uint8_t r;
 uint8_t g;
 uint8_t b;

 operator bool() const {
   return (r | g | b) != 0;
 }
};

using map_t = std::vector<std::vector<tile_t>>;

struct instn_t {
  Direction d;
  int8_t count;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct cursor_t {
  int row;
  int column;
};

struct instn2_t {
  Direction d;
  int64_t count;
};


void moveCursor(Direction dir, cursor_t& cursor) {
  using enum Direction;
  if (dir == Up) --cursor.row;
  if (dir == Down) ++cursor.row;
  if (dir == Left) --cursor.column;
  if (dir == Right) ++cursor.column;
}

void colorAtCursor(map_t& digMap, const cursor_t cursor, const instn_t inst) {
  digMap.at(cursor.row).at(cursor.column).r = inst.r;
  digMap.at(cursor.row).at(cursor.column).g = inst.g;
  digMap.at(cursor.row).at(cursor.column).b = inst.b;
}

uint8_t parseColorCode(std::string_view line) {
  // TODO: lol, jk, glad I didn't bother with that nonsense!
  return 128;
}
instn_t parseInstruction(std::string_view line) {
  instn_t result;
  if (line.front() == 'U') result.d = Direction::Up;
  else if (line.front() == 'R') result.d = Direction::Right;
  else if (line.front() == 'D') result.d = Direction::Down;
  else if (line.front() == 'L') result.d = Direction::Left;
  line.remove_prefix(2);
  result.count = utils::parseInt(line);
  utils::eatLiteral(" (#", line);

  result.r = parseColorCode(line);
  result.g = parseColorCode(line);
  result.b = parseColorCode(line);
  return result;
}

uint64_t countSquares(const map_t& digMap) {
  uint64_t result = 0;
  for (const auto& row : digMap) {
    for (const auto& tile : row) {
      result += ((tile.r | tile.b | tile.g) > 0);
    }
  }
  return result;
}

void extendDown(map_t& digMap, int count) {
  auto rowLen = digMap.front().size();
  while (--count >= 0) {
    digMap.emplace_back(rowLen, tile_t{0,0,0});
  }
}

void extendRight(map_t& digMap, int count) {
  auto rowLen = digMap.front().size();
  for (auto& row : digMap) {
    for (int c = 0; c < count; ++c) {
      row.emplace_back(tile_t{0,0,0});
    }
  }
}

void extendUp(map_t& digMap, int count, cursor_t& cursor) {
  cursor.row += count;
  auto rowLen = digMap.front().size();
  while (--count >= 0) {
    digMap.emplace(digMap.begin(), rowLen, tile_t{0,0,0});
  }
}

void extendLeft(map_t& digMap, int count, cursor_t& cursor) {
  cursor.column += count;
  auto rowLen = digMap.front().size();
  for (auto& row : digMap) {
    for (int c = 0; c < count; ++c) {
      row.emplace(row.begin(), tile_t{0,0,0});
    }
  }
}

void executeInstruction(map_t& digMap, cursor_t& cursor, instn_t instruction) {
  if (instruction.d == Direction::Up) {
    if (cursor.row - instruction.count < 0) {
      extendUp(digMap, instruction.count + 1, cursor);
    }
  } else if (instruction.d == Direction::Right) {
    if (cursor.column + instruction.count >= digMap.front().size()) {
      extendRight(digMap, instruction.count + 1); // slightly overdoes it, but hopefully a bit more efficient
    }
  } else if (instruction.d == Direction::Down) {
    if (cursor.row + instruction.count >= digMap.size()) {
      extendDown(digMap, instruction.count + 1); // slightly overdoes it, but hopefully a bit more efficient
    }
  } else if (instruction.d == Direction::Left) {
    if (cursor.column - instruction.count < 0) {
      extendLeft(digMap, instruction.count + 1, cursor);
    }
  }
  while (--instruction.count >= 0) {
    moveCursor(instruction.d, cursor);
    colorAtCursor(digMap, cursor, instruction);
  }
}

void debugPrintMap(const map_t& digMap) {
  for (const auto& row : digMap) {
    for (const auto& tile : row) {
      fmt::print("{}", (tile) ? '#' : '.');
    }
    fmt::println("");
  }
}

void debugPrintMap(const map_t& digMap, cursor_t cursor) {
  for (int r = 0; r < digMap.size(); ++r) {
    for (int c = 0; c <= digMap[r].size(); ++c) {
      if (r == cursor.row && c == cursor.column) {
        fmt::print("X");
      } else {
        auto tile = digMap[r][c];
        fmt::print("{}", (tile) ? '#' : '.');
      }
    }
    fmt::println("");
  }
}

template<bool Dug>
typename std::vector<tile_t>::iterator findNext(typename std::vector<tile_t>::iterator it, const std::vector<tile_t>& row) {
  while (it != row.end() && (*it != Dug)) {
    ++it;
  }
  return it;
}

int findStartingRow(const map_t& map) {
  // We want to find a row with a single # at the front
  for (int r = 0; r < map.size(); ++r) {
    if (map[r][0] && !map[r][1]) {
      return r;
    }
  }
  return -1;
}

void fillMap(map_t& digMap) {
  auto startCell = cursor_t{findStartingRow(digMap), 1};
  const instn_t color = instn_t{Direction::Up, 1, 255,255,255};
  static constexpr std::array<Direction, 4> dirs = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};

  std::vector<cursor_t> q;
  q.push_back(startCell);

  while (!q.empty()) {
    auto cursor = q.back();
    q.pop_back();
    colorAtCursor(digMap, cursor, color);
    for (const auto dir : dirs) {
      auto cprime = cursor;
      moveCursor(dir, cprime);
      if (cprime.column >= 0 && cprime.column < digMap[0].size()
          && cprime.row >= 0 && cprime.row < digMap.size()) {
        if (!digMap.at(cprime.row).at(cprime.column)) {
          q.push_back(cprime);
        }
      }
    }
  }
}

void test() {
  map_t dm = {{tile_t{128,128,128}}};

  extendRight(dm, 5);
  utils::Assert(dm.size() == 1);
  utils::Assert(dm.front().size() == 6);
  extendDown(dm, 3);
  utils::Assert(dm.size() == 4);
  utils::Assert(dm.front().size() == 6);
  utils::Assert(dm.back().size() == 6);

  cursor_t cur = {0,0};
  extendUp(dm, 2, cur);
  utils::Assert(dm.size() == 6);
  utils::Assert(cur.column == 0);
  utils::Assert(cur.row == 2);
  utils::Assert(dm.front().size() == 6);
  utils::Assert(dm.back().size() == 6);

  extendLeft(dm, 4, cur);
  utils::Assert(dm.size() == 6);
  utils::Assert(cur.column == 4);
  utils::Assert(cur.row == 2);
  utils::Assert(dm.front().size() == 10);
  utils::Assert(dm.back().size() == 10);

  //////
  std::vector<tile_t> row{10, tile_t{128,128,128}};
  utils::AssertEq(row.size(), 10ul);
  row[0] = tile_t{0,0,0};
  row[1] = tile_t{0,0,0};
  row[2] = tile_t{0,0,0};
  row[4] = tile_t{0,0,0};
  row[5] = tile_t{0,0,0};
  row[6] = tile_t{0,0,0};
  row[9] = tile_t{0,0,0};
  // row == ...#...##.
  auto it = row.begin();
  auto nextIt = findNext<false>(it, row);
  utils::AssertEq(it - row.begin(), nextIt - row.begin());
  nextIt = findNext<true>(nextIt, row);
  utils::AssertEq(nextIt - row.begin(), 3l);

}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day18_test.txt"};
  utils::LineReader lr{"inp/day18.txt"};

  map_t digMap = {{tile_t{128,128,128}}};
  auto cursor = cursor_t{0,0};
  while (auto line = lr.getLine()) {
    auto inst = parseInstruction(*line);
    executeInstruction(digMap, cursor, inst);
  }

  fillMap(digMap);
  // debugPrintMap(digMap, cursor);

  auto p1 = countSquares(digMap);
  auto p2 = 0;

  fmt::println("Day18: Part 1: {}", p1);
  fmt::println("Day18: Part 2: {}", p2);
}
