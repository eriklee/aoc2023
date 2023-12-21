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

struct instn_t {
  Direction d;
  int64_t count;
};

struct cursor_t {
  int64_t row;
  int64_t column;
};

cursor_t moveCursor(instn_t inst, const cursor_t& cursor) {
  auto result = cursor;
  using enum Direction;
  if (inst.d == Up) result.row -= inst.count;
  if (inst.d == Down) result.row += inst.count;;
  if (inst.d == Left) result.column -= inst.count;
  if (inst.d == Right) result.column += inst.count;;
  return result;
}

instn_t parseInstruction1(std::string_view& line) {
  instn_t result;
  if (line.front() == 'U') result.d = Direction::Up;
  else if (line.front() == 'R') result.d = Direction::Right;
  else if (line.front() == 'D') result.d = Direction::Down;
  else if (line.front() == 'L') result.d = Direction::Left;
  line.remove_prefix(2);
  result.count = utils::parseInt(line);
  utils::eatLiteral(" ", line);
  return result;
}

// must be called after parseInstruction1
instn_t parseInstruction2(std::string_view line) {
  instn_t result;
  utils::AssertEq(line.back(), ')');
  line.remove_suffix(1);
  if (line.back() == '3') result.d = Direction::Up;
  else if (line.back() == '0') result.d = Direction::Right;
  else if (line.back() == '1') result.d = Direction::Down;
  else if (line.back() == '2') result.d = Direction::Left;
  line.remove_suffix(1);
  utils::eatLiteral("(#", line);
  result.count = utils::parseHexInt(line);
  return result;
}

class AreaFinder {
  public:
    AreaFinder() = default;
    void addPoint(instn_t inst) {
      auto lastCur = m_cursor;
      m_cursor = moveCursor(inst, m_cursor);
      // fmt::println("i={},{} last={},{} new={},{}", fmt::underlying(inst.d), inst.count, lastCur.column, lastCur.row, m_cursor.column, m_cursor.row);
      m_running_total += (lastCur.column * m_cursor.row - lastCur.row * m_cursor.column);
      m_running_total += inst.count;
    }
    int64_t finalize() {
      utils::AssertEq(m_cursor.row, 0l);
      utils::AssertEq(m_cursor.column, 0l);
      return std::abs(m_running_total / 2);
    }
  private:
    cursor_t m_cursor{0,0};
    int64_t m_running_total = 2;
};

void test() {
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day18_test.txt"};
  utils::LineReader lr{"inp/day18.txt"};

  AreaFinder af1;
  AreaFinder af2;
  while (auto line = lr.getLine()) {
    auto inst = parseInstruction1(*line);
    auto inst2 = parseInstruction2(*line);
    af1.addPoint(inst);
    af2.addPoint(inst2);
  }

  auto p1 = af1.finalize();
  auto p2 = af2.finalize();

  fmt::println("Day18: Part 1: {}", p1);
  fmt::println("Day18: Part 2: {}", p2);
}
