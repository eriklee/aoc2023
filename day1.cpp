#include <cassert>
#include <fstream>
#include <fmt/core.h>
#include <regex>
#include <string>
#include <string_view>

int getCalibrationValuePart1(const std::string& line) {
  int ret = (line.at(line.find_first_of("0123456789")) - '0') * 10;
  ret += line.at(line.find_last_of("0123456789")) - '0';
  return ret;
}

int matchToVal(std::string_view m) {
  if (m.size() == 1) return m.front() - '0';
  if (m == "one") return 1;
  if (m == "two") return 2;
  if (m == "three") return 3;
  if (m == "four") return 4;
  if (m == "five") return 5;
  if (m == "six") return 6;
  if (m == "seven") return 7;
  if (m == "eight") return 8;
  if (m == "nine") return 9;
  throw std::runtime_error(fmt::format("match wasn't a valid value! m={}", m));
}

int getCalibrationValuePart2(std::string line) {
  std::regex cv_regex("\\d|(one)|(two)|(three)|(four)|(five)|(six)|(seven)|(eight)|(nine)");
  int ret = 0;
  int last = 0;
  for (std::smatch sm; std::regex_search(line, sm, cv_regex);) {
    // fmt::println("found match={}", sm.str());
    const auto val = matchToVal(sm.str());
    // The example on cppreference is actually a bit broken. oops!
    line = line.substr(sm.prefix().length() + 1);
    if (ret == 0) ret = val * 10;
    last = val;
  }
  return ret + last;
}

void test() {
  std::string l = "three98oneightzn";
  assert(getCalibrationValuePart2(l) == 38);
}

int main(int argc, char **argv) {

  // test();
  std::string l;
  {
    // std::ifstream file("inp/day1_test.txt");
    // std::ifstream file("inp/day1_test2.txt");
    std::ifstream file("inp/day1.txt");
    uint64_t sum1 = 0;
    uint64_t sum2 = 0;
    while (std::getline(file, l)) {
      auto calibration_value1 = getCalibrationValuePart1(l);
      sum1 += calibration_value1;
      auto calibration_value2 = getCalibrationValuePart2(l);
      sum2 += calibration_value2;
      // fmt::println("l={} cv1={} cv2={}", l, calibration_value1, calibration_value2);
    }
    fmt::println("Day1: Part 1: {}", sum1);
    fmt::println("Day1: Part 2: {}", sum2);
  }
}
