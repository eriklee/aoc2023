#include "utils.hpp"

#include <fmt/format.h>
#include <unordered_map>
#include <vector>
#include <string_view>

struct rule_t {
  char rating;
  char comp;
  int num;
  std::string target_wf;
};

struct workflow_t {
  std::string name;
  std::vector<rule_t> rules;
};

struct ratings_t {
  int x;
  int m;
  int a;
  int s;
};

using wfs_t = std::unordered_map<std::string, std::vector<rule_t>>;

rule_t parseRule(std::string_view& line) {
  rule_t result;

  std::string_view rule_part = line;
  auto comma = rule_part.find(',');
  if (comma == rule_part.npos) {
    utils::AssertEq(rule_part.back(), '}');
    rule_part.remove_suffix(1);
    line.remove_prefix(line.size());
  } else {
    rule_part = rule_part.substr(0, comma);
    line.remove_prefix(comma);
    utils::Assert(utils::eatLiteral(",", line));
  }

  if (auto colon_pos = rule_part.find(':'); colon_pos == rule_part.npos) {
    result.rating = 'x'; result.comp = '>'; result.num = -1; result.target_wf = rule_part;
  } else {
    result.rating = rule_part.front();
    rule_part.remove_prefix(1);
    result.comp = rule_part.front();
    rule_part.remove_prefix(1);
    result.num = utils::parseInt(rule_part);
    utils::Assert(utils::eatLiteral(":", rule_part));

    result.target_wf = rule_part;
  }
  return result;
}

workflow_t parseWorkflow(std::string_view line) {
  workflow_t result;
  auto brace_pos = line.find('{');
  result.name = line.substr(0, brace_pos);
  line.remove_prefix(brace_pos);
  utils::Assert(utils::eatLiteral("{", line));
  while (!line.empty()) {
    result.rules.push_back(parseRule(line));
  }

  return result;
}

ratings_t parsePart(std::string_view line) {
  ratings_t result;
  utils::Assert(utils::eatLiteral("{x=", line));
  result.x = utils::parseInt(line);
  utils::Assert(utils::eatLiteral(",m=", line));
  result.m = utils::parseInt(line);
  utils::Assert(utils::eatLiteral(",a=", line));
  result.a = utils::parseInt(line);
  utils::Assert(utils::eatLiteral(",s=", line));
  result.s = utils::parseInt(line);
  utils::Assert(utils::eatLiteral("}", line));
  utils::Assert(line.empty());

  return result;
}

bool ruleMatches(const rule_t& rule, const ratings_t& part) {
  int part_val;
  if (rule.rating == 'x') part_val = part.x;
  else if (rule.rating == 'm') part_val = part.m;
  else if (rule.rating == 'a') part_val = part.a;
  else if (rule.rating == 's') part_val = part.s;
  else __builtin_unreachable();

  if (rule.comp == '>') return part_val > rule.num;
  else if (rule.comp == '<') return part_val < rule.num;

  __builtin_unreachable();
}

bool acceptPart(const wfs_t& workflows, const ratings_t& part) {
  std::string current_wf = "in";
  while (current_wf != "A" && current_wf != "R") {
    const auto& rules = workflows.at(current_wf);
    for (const auto& rule : rules) {
      if (ruleMatches(rule, part)) {
        current_wf =  rule.target_wf;
        break;
      }
    }
  }
  return current_wf == "A";
}

int64_t totalRatings(const ratings_t& part) {
  return part.x + part.m + part.a + part.s;
}

struct rating_bounds_t {
  int x_min = 1;
  int x_max = 4000;
  int m_min = 1;
  int m_max = 4000;
  int a_min = 1;
  int a_max = 4000;
  int s_min = 1;
  int s_max = 4000;
};

struct state_t {
  rating_bounds_t ratings;
  std::string wf_name;
};

bool validState(const state_t& st) {
  if (st.wf_name == "R") return false;

  return (st.ratings.x_min <= st.ratings.x_max)
    && (st.ratings.m_min <= st.ratings.m_max)
    && (st.ratings.a_min <= st.ratings.a_max)
    && (st.ratings.s_min <= st.ratings.s_max);
}

std::pair<state_t, state_t> splitStateForRule(state_t state, rule_t rule) {
  if (rule.num == -1) {
    state.wf_name = rule.target_wf;
    auto state2 = state;
    state2.ratings.m_min = 4001;
    return {state, state2};
  }
  if (rule.rating == 'x') {
    if (rule.comp == '>') {
      auto state2 = state;
      state.ratings.x_min = rule.num + 1;
      state.wf_name = rule.target_wf;
      state2.ratings.x_max = rule.num;
      return {state, state2};
    }
    else if (rule.comp == '<') {
      auto state2 = state;
      state.ratings.x_max = rule.num - 1;
      state.wf_name = rule.target_wf;
      state2.ratings.x_min = rule.num;
      return {state, state2};
    }
  }
  else if (rule.rating == 'm') {
    if (rule.comp == '>') {
      auto state2 = state;
      state.ratings.m_min = rule.num + 1;
      state.wf_name = rule.target_wf;
      state2.ratings.m_max = rule.num;
      return {state, state2};
    }
    else if (rule.comp == '<') {
      auto state2 = state;
      state.ratings.m_max = rule.num - 1;
      state.wf_name = rule.target_wf;
      state2.ratings.m_min = rule.num;
      return {state, state2};
    }
  }
  else if (rule.rating == 'a'){
    if (rule.comp == '>') {
      auto state2 = state;
      state.ratings.a_min = rule.num + 1;
      state.wf_name = rule.target_wf;
      state2.ratings.a_max = rule.num;
      return {state, state2};
    }
    else if (rule.comp == '<') {
      auto state2 = state;
      state.ratings.a_max = rule.num - 1;
      state.wf_name = rule.target_wf;
      state2.ratings.a_min = rule.num;
      return {state, state2};
    }
  }
  else if (rule.rating == 's'){
    if (rule.comp == '>') {
      auto state2 = state;
      state.ratings.s_min = rule.num + 1;
      state.wf_name = rule.target_wf;
      state2.ratings.s_max = rule.num;
      return {state, state2};
    }
    else if (rule.comp == '<') {
      auto state2 = state;
      state.ratings.s_max = rule.num - 1;
      state.wf_name = rule.target_wf;
      state2.ratings.s_min = rule.num;
      return {state, state2};
    }
  }
  __builtin_unreachable();
}

std::vector<state_t> nextStates(const std::vector<rule_t>& rules, state_t state) {
  std::vector<state_t> result;
  for (const auto rule : rules) {
    const auto [s1,s2] = splitStateForRule(state, rule);
    if (validState(s1)) result.push_back(s1);
    if (!validState(s2))
      break;
    state = s2;
  }

  return result;
}

uint64_t countAcceptance(const state_t state) {
  uint64_t result = 1;
  result *= state.ratings.x_max - state.ratings.x_min + 1;
  result *= state.ratings.m_max - state.ratings.m_min + 1;
  result *= state.ratings.a_max - state.ratings.a_min + 1;
  result *= state.ratings.s_max - state.ratings.s_min + 1;
  return result;
}

uint64_t part2(const wfs_t& workflows) {
  state_t initial_state = state_t{.ratings=rating_bounds_t{}, .wf_name="in"};
  std::vector<state_t> q;
  q.push_back(initial_state);
  uint64_t result = 0;
  while (!q.empty()) {
    auto state = q.back();
    q.pop_back();
    if (state.wf_name == "A") {
      // fmt::println("{} x{}/{} m{}/{} a{}/{} s{}/{}", state.wf_name, state.ratings.x_min, state.ratings.x_max, state.ratings.m_min, state.ratings.m_max, state.ratings.a_min, state.ratings.a_max, state.ratings.s_min, state.ratings.s_max);
      // fmt::println("acceptances: {}", countAcceptance(state));
      result += countAcceptance(state);
      continue;
    }
    const auto& wf = workflows.at(state.wf_name);
    auto next = nextStates(wf, state);
    q.insert(q.end(), next.begin(), next.end());
  }

  return result;
}

void test() {
  auto part = parsePart("{x=787,m=2655,a=1222,s=2876}");
  utils::AssertEq(part.x, 787);
  utils::AssertEq(part.m, 2655);
  utils::AssertEq(part.a, 1222);
  utils::AssertEq(part.s, 2876);

  using namespace std::literals::string_view_literals;
  auto rulestr = "a<2006:qkq,m>2090:A,rfg}"sv;
  auto r = parseRule(rulestr);
  utils::AssertEq(r.rating, 'a');
  utils::AssertEq(r.comp, '<');
  utils::AssertEq(r.num, 2006);
  utils::Assert(r.target_wf == "qkq");
  ratings_t rat = ratings_t{.x=1,.m=1,.a=2000,.s=1};
  utils::Assert(ruleMatches(r, rat));

  r = parseRule(rulestr);
  utils::AssertEq(r.rating, 'm');
  utils::AssertEq(r.comp, '>');
  utils::AssertEq(r.num, 2090);
  utils::Assert(r.target_wf == "A");

  r = parseRule(rulestr);
  utils::AssertEq(r.num, -1);
  utils::Assert(r.target_wf == "rfg");

  auto wf_str = "px{a<2006:qkq,m>2090:A,rfg}"sv;
  auto wf = parseWorkflow(wf_str);
  utils::Assert(wf.name == "px");
  utils::Assert(wf.rules.size() == 3);
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day19_test.txt"};
  utils::LineReader lr{"inp/day19.txt"};

  wfs_t workflows;
  while (auto line = lr.getLine()) {
    if (line->empty()) break;
    auto workflow = parseWorkflow(*line);
    workflows.insert({workflow.name, workflow.rules});
  }

  auto p1 = 0l;
  while (auto line = lr.getLine()) {
    auto part = parsePart(*line);
    auto accepted = acceptPart(workflows, part);
    if (accepted) {
      // fmt::println("Accepted = {}", *line);
      p1 += totalRatings(part);
    }
  }

  auto p2 = part2(workflows);

  fmt::println("Day19: Part 1: {}", p1);
  fmt::println("Day19: Part 2: {}", p2);
}
