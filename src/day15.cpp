#include "utils.hpp"

#include <algorithm>
#include <fmt/format.h>
#include <vector>
#include <string_view>

uint8_t calculateHASH(std::string_view sv) {
  uint8_t result = 0;
  for (const char c : sv) {
    result += c;
    result *= 17;
  }
  return result;
}

void test() {
  auto hashhash = calculateHASH("HASH");
  utils::AssertEq(static_cast<int>(hashhash), 52);
}

std::optional<std::string_view> getSequence(std::string_view& line) {
  if (line.empty()) return {};

  auto breakidx = line.find(',');
  std::string_view result = line.substr(0,breakidx);
  line.remove_prefix(std::min(line.size(), result.size() + 1));
  return result;
}

std::string_view getLabel(std::string_view& line) {
  if (line.back() == '-') {
    return line.substr(0, line.size() - 1);
  } else {
    auto breakidx = line.find('=');
    std::string_view result = line.substr(0,breakidx);
    return result;
  }
}

std::optional<int> getFocalLength(std::string_view& line) {
  if (line.back() == '-') {
    return {};
  } else {
    auto breakidx = line.find('=');
    auto intpart = line.substr(breakidx+1, line.size());
    return utils::parseInt(intpart);
  }
}

struct lens_t {
  std::string_view label;
  uint8_t focal_length;
};

using box_t = std::vector<lens_t>;

using boxes_t = std::array<box_t, 256>;

void dashAction(box_t& box, std::string_view label) {
  auto slot = std::find_if(box.begin(), box.end(), [&](const auto lens) { return lens.label == label; });
  if (slot == box.end()) return;
  box.erase(slot);
}

void equalsAction(box_t& box, std::string_view label, uint8_t focalLength) {
  auto slot = std::find_if(box.begin(), box.end(), [&](const auto lens) { return lens.label == label; });
  if (slot == box.end())
    box.push_back(lens_t{.label=label, .focal_length=focalLength});
  else slot->focal_length = focalLength;
}

void applyStep(boxes_t& boxes, std::string_view step) {
  auto label = getLabel(step);
  auto flen = getFocalLength(step);

  auto& box = boxes.at(calculateHASH(label));
  // fmt::println("{} -> label={} flen={} box={}", step, label, flen.value_or(0), calculateHASH(label));
  if (flen) {
    equalsAction(box, label, *flen);
  } else {
    dashAction(box, label);
  }
}

uint64_t calculateFocusingPower(const boxes_t& boxes) {
  uint64_t result = 0;
  for (int box = 0; box < boxes.size(); ++box) {
    for (int slot = 0; slot < boxes[box].size(); ++slot) {
      auto focusing_power = box + 1;
      focusing_power *= slot + 1;
      focusing_power *= boxes[box][slot].focal_length;
      // fmt::println("{} {} * {} * {} = {}", boxes[box][slot].label, box + 1, slot + 1, boxes[box][slot].focal_length, focusing_power);
      result += focusing_power;
    }
  }
  return result;
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day15_test.txt"};
  utils::LineReader lr{"inp/day15.txt"};

  uint64_t p1 = 0;
  boxes_t boxes;

  auto line = lr.getLine();
  while (auto step = getSequence(*line)) {
    p1 += calculateHASH(*step);
    // fmt::println("{} becomes {}.", *step, calculateHASH(*step));
    applyStep(boxes, *step);
  }

  fmt::println("Day15: Part 1: {}", p1);
  auto p2 = calculateFocusingPower(boxes);
  fmt::println("Day15: Part 2: {}", p2);
}
