#include "utils.hpp"

#include <algorithm>
#include <fmt/format.h>
#include <numeric>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <map>

struct Node {
  std::string name;
  std::string left;
  std::string right;
};

template <> struct fmt::formatter<Node> {
  constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
    return ctx.end();
  }
  auto format(const Node& r, format_context& ctx) const -> format_context::iterator {
    return fmt::format_to(ctx.out(), "{} = ({}, {})", r.name, r.left, r.right);
  }
};

using graph_t = std::unordered_map<std::string, Node>;

Node parseNode(std::string_view line) {
  Node result;
  result.name = utils::readWord(line);
  utils::Assert(utils::eatLiteral(" = (", line));
  result.left = utils::readWord(line);
  result.left = result.left.substr(0, 3);
  utils::Assert(utils::eatLiteral(" ", line));
  result.right = utils::readWord(line);
  result.right = result.right.substr(0, 3);
  return result;
}

template<bool Debug = false>
int64_t countSteps(std::string_view inst, const graph_t& graph) {
  int64_t result = 0;
  size_t idx = 0;
  std::string current_node = "AAA";
  while (current_node != "ZZZ") {
    if constexpr (Debug) fmt::print("Going from {}, {} -> ", current_node, inst[idx]);
    switch (inst[idx]) {
      case 'L': current_node = graph.at(current_node).left;
                break;
      case 'R': current_node = graph.at(current_node).right;
                break;
    }
    if constexpr (Debug) fmt::println("{}", current_node);
    idx = (idx + 1) % inst.size();
    ++result;
  }
  return result;
}

bool atExit(const std::string& n) {
  return n.back() == 'Z';
}

struct search_state_t {
  int idx;
  std::string node;
  size_t stepCount = 0;
  auto operator<=>(const search_state_t&) const = default;
};

using z_graph = std::map<search_state_t, search_state_t>;

template<bool Debug>
search_state_t countStepsPart2(std::string_view inst, const graph_t& graph, std::string startNode, int startIdx) {
  search_state_t result { .idx = startIdx, .node = startNode, .stepCount = 0};

  do {
    if constexpr (Debug) fmt::print("Going from {}, {} -> ", result.node, inst[result.idx]);
    switch (inst[result.idx]) {
      case 'L': result.node = graph.at(result.node).left;
                break;
      case 'R': result.node = graph.at(result.node).right;
                break;
    }
    if constexpr (Debug) {fmt::println("{}", result.node);}

    result.idx = (result.idx + 1) % inst.size();
    ++result.stepCount;
  } while (!atExit(result.node));
  return result;
}

template<bool Debug>
size_t part2(std::string_view inst, const graph_t& graph) {
  z_graph zg;
  std::vector<search_state_t> currentNodes;
  for (const auto [name, _node] : graph)
    { if (name.back() == 'A') currentNodes.push_back({.idx = 0, .node = name, .stepCount = 0}); }

  std::vector<size_t> loopLengths;

  for (auto& it : currentNodes) {
    auto current_state = it;
    while (!zg.contains(current_state)) {
        auto next = countStepsPart2<Debug>(inst, graph, current_state.node, current_state.idx);
        zg.emplace(current_state, next);
        it.idx = next.idx;
        it.node = next.node;
        it.stepCount += next.stepCount;
        current_state = it;
        current_state.stepCount = 0;
    }
  }
  for (auto current_state : currentNodes) {
    auto mapped_cs = current_state;
    mapped_cs.stepCount = 0;
    auto next = zg.at(mapped_cs);
    if constexpr (Debug) fmt::println("Using cached result for {},{} -> {} -> {} in {}", current_state.node, current_state.idx, next.node, next.idx, next.stepCount);
    utils::Assert(next.node == current_state.node); // This seems to be true, but is not generally the case :/
    utils::Assert(current_state.stepCount % next.stepCount == 0); // This seems to be true, but is not generally the case :/
    loopLengths.push_back(next.stepCount);
  }

  size_t result = loopLengths.front();
  for (const auto l : loopLengths) { result = std::lcm(result, l); }
  return result;
}

void test() {
  auto n = parseNode("AAA = (BBB, CCC)");
  utils::Assert(n.name == "AAA");
  utils::Assert(n.left == "BBB");
  utils::Assert(n.right == "CCC");
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day8_test1.txt"};
  // utils::LineReader lr{"inp/day8_test3.txt"};
  utils::LineReader lr{"inp/day8.txt"};
  std::string instructions{*lr.getLine()};
  utils::Assert(lr.getLine()->empty());
  graph_t graph;

  while (auto line = lr.getLine()) {
    auto n = parseNode(*line);
    // fmt::println("{}", n);
    graph.emplace(n.name, n);
  }
  auto p1 = countSteps<false>(instructions, graph);
  fmt::println("Day8: Part 1: {}", p1);
  auto p2 = part2<false>(instructions, graph);
  fmt::println("Day8: Part 2: {}", p2);
}
