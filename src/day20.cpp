#include "utils.hpp"

#include <deque>
#include <fmt/format.h>
#include <unordered_map>
#include <vector>
#include <string_view>

enum class ModType {
  Plain,
  FlipFlopOff,
  FlipFlopOn,
  Conjunction,
};

enum class Pulse : bool {
  Low = false,
  High = true,
};

struct module_t {
  std::string name;
  ModType typ;
  std::unordered_map<std::string, Pulse> inputs;
  std::vector<std::string> outputs;
};

struct pulse_t {
  std::string from;
  std::string to;
  Pulse val;
};

ModType parseModType(std::string_view& line) {
  if (utils::eatLiteral("%", line)) return ModType::FlipFlopOff;
  else if (utils::eatLiteral("&", line)) return ModType::Conjunction;
  else return ModType::Plain;
}

module_t parseModule(std::string_view line) {
  module_t result;
  result.typ = parseModType(line);
  result.name = utils::readWord(line);
  utils::Assert(utils::eatLiteral(" -> ", line));
  while (!line.empty()) {
    result.outputs.emplace_back(utils::readToChar(',', line));
    utils::eatLiteral(", ", line);
  }

  return result;
}

void fixupModuleConnections(std::unordered_map<std::string, module_t>& mods) {
  for (const auto& [modName, mod] : mods) {
    for (const auto& outName : mod.outputs) {
      if (!mods.contains(outName)) 
        mods.emplace(outName, module_t{outName, ModType::Plain, {}, {}});
      mods.at(outName).inputs.emplace(modName, Pulse::Low);
    }
  }
}

void test() {
  using namespace std::literals::string_view_literals;
  {
  auto line = "broadcaster -> a, b, c"sv;
  auto mod = parseModule(line);
  utils::Assert(mod.name == "broadcaster");
  utils::Assert(mod.typ == ModType::Plain);
  utils::AssertEq(mod.outputs.size(), 3ul);
  utils::AssertEq(mod.outputs[0], {"a"});
  utils::AssertEq(mod.outputs[1], {"b"});
  utils::AssertEq(mod.outputs[2], {"c"});
  }
  {
  auto line = "&inv -> a"sv;
  auto mod = parseModule(line);
  utils::Assert(mod.name == "inv");
  utils::Assert(mod.typ == ModType::Conjunction);
  utils::AssertEq(mod.outputs.size(), 1ul);
  utils::AssertEq(mod.outputs[0], {"a"});
  }
}

using pulse_q = std::deque<pulse_t>;

std::vector<pulse_t> deliverPulse(module_t& mod, const pulse_t& p) {
  std::vector<pulse_t> result;
  if (mod.typ == ModType::Plain) {
    for (const auto& out : mod.outputs)
      result.emplace_back(mod.name, out, p.val);
  } else if (mod.typ == ModType::FlipFlopOff) {
    if (p.val == Pulse::Low) {
      mod.typ = ModType::FlipFlopOn;
      for (const auto& out : mod.outputs)
        result.emplace_back(mod.name, out, Pulse::High);
    }
  } else if (mod.typ == ModType::FlipFlopOn) {
    if (p.val == Pulse::Low) {
      mod.typ = ModType::FlipFlopOff;
      for (const auto& out : mod.outputs)
        result.emplace_back(mod.name, out, Pulse::Low);
    }
  } else if (mod.typ == ModType::Conjunction) {
    mod.inputs.at(p.from) = p.val;
    auto pv = Pulse::Low;
    for (const auto& [_n, val] : mod.inputs) {
      if (val == Pulse::Low) pv = Pulse::High;
    }

    for (const auto& out : mod.outputs)
      result.emplace_back(mod.name, out, pv);
  }
  return result;
}

int64_t part1(std::unordered_map<std::string, module_t> mods) {
  int64_t highs = 0; int64_t lows = 0;
  pulse_q q;
  for (int i = 0; i < 1000; ++i) {
    q.emplace_back("button", "broadcaster", Pulse::Low);
    while (!q.empty()) {
      auto pulse = q.front();
      q.pop_front();
      pulse.val == Pulse::High ? ++highs : ++lows;
      // fmt::println("{} -{}-> {}", pulse.from, (pulse.val == Pulse::High ? "high" : "low"), pulse.to);
      auto& mod = mods.at(pulse.to);
      auto morePulses = deliverPulse(mod, pulse);
      q.insert(q.end(), morePulses.begin(), morePulses.end());
    }
  }

  return highs * lows;
}

int64_t part2(std::unordered_map<std::string, module_t> mods) {
  // Having a look at the graph there are 4 subgraphs that all feed into
  // a conj result (rm). Some slight hinting suggested they might all be
  // counters and the somewhat on-brand implication is that their periods
  // can all be multiplied together to get the result.
  int64_t buttonPushes = 0;
  const int goalCount = mods.at("broadcaster").outputs.size();
  std::unordered_map<std::string, uint64_t> periods;
  pulse_q q;
  while (++buttonPushes) {
    q.emplace_back("button", "broadcaster", Pulse::Low);
    while (!q.empty()) {
      auto pulse = q.front();
      q.pop_front();
      if (pulse.to == "rm" && pulse.val == Pulse::High) {
        if (!periods.contains(pulse.from)) {
          // fmt::println("bc={} {} -{}-> {}", buttonPushes, pulse.from, (pulse.val == Pulse::High ? "high" : "low"), pulse.to);
          periods.emplace(pulse.from, buttonPushes);
        }
        if (periods.size() == 4) {
          int64_t result = 1;
          for (const auto [_, v] : periods) result *= v;
          return result;
        }
      }
      auto& mod = mods.at(pulse.to);
      auto morePulses = deliverPulse(mod, pulse);
      q.insert(q.end(), morePulses.begin(), morePulses.end());
    }
  }

  return buttonPushes;
}

void dotPart2(const std::unordered_map<std::string, module_t>& mods) {
  fmt::println("digraph {{ ");
  for (const auto& [n, mod] : mods) {
    auto lbl = n;
    auto color = "black";
    if (mod.typ == ModType::Conjunction) {
      lbl = "&" + lbl;
      color = "green";
    }
    if (mod.typ == ModType::FlipFlopOff){
      lbl = "%" + lbl;
      color = "red";
    }
    fmt::println("node [xlabel=\"{}\", color={}];", lbl, color);
    fmt::println("{} -> {{{}}};", n, fmt::join(mod.outputs, ", "));
  }

  fmt::println(" }}");
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day20_test1.txt"};
  // utils::LineReader lr{"inp/day20_test2.txt"};
  utils::LineReader lr{"inp/day20.txt"};

  std::unordered_map<std::string, module_t> mods;
  while (auto line = lr.getLine()) {
    auto mod = parseModule(*line);
    mods.insert({mod.name, mod});
  }

  fixupModuleConnections(mods);
  auto p1 = part1(mods);

  // dotPart2(mods);
  auto p2 = part2(mods);

  fmt::println("Day20: Part 1: {}", p1);
  fmt::println("Day20: Part 2: {}", p2);
}
