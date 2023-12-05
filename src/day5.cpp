#include "utils.hpp"

#include <algorithm>
#include <fmt/format.h>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_set>

struct range {
  int64_t start;
  int64_t length;
};
template <> struct fmt::formatter<range> {
  constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
    return ctx.end();
  }
  auto format(const range& r, format_context& ctx) const -> format_context::iterator {
    return fmt::format_to(ctx.out(), "[{}; {}]", r.start, r.length);
  }
};
struct range_mapping {
  // the results of applying a mapping to a range have 4 cases
  // 1. the range does not overlap with the mapping at all. 
  //    initial range returned in prefix, others have length 0
  // 2. the range is entirely covered by the mapping
  //    result range has the mapped range, others have length 0
  // 3. range is larger than the mapping on one side
  //    result will cover the overlapping result, suffix or prefix will have the rest
  // 4. range is bigger on both sides
  //    all three ranges will have values
  range prefix;
  range suffix;
  range result;
};

struct mapping {
  int64_t dest_range_start;
  int64_t source_range_start;
  int64_t source_range_len;
};

using numbers = std::vector<int64_t>;
using number_ranges = std::vector<range>;

std::optional<int64_t> applyMapping(int64_t n, const mapping& mapping) {
  auto distance = n - mapping.source_range_start;
  if (distance > 0 && distance < mapping.source_range_len)
    return mapping.dest_range_start + distance;
  return std::nullopt;
}

range_mapping applyMapping(range n, const mapping& mapping) {
  range_mapping rm;
  // do prefix
  auto distance = n.start - mapping.source_range_start;
  if (distance < 0) {
    rm.prefix = {n.start, std::min(n.length, -distance)};
    n.start += rm.prefix.length;
    n.length -= rm.prefix.length;
  } else {
    rm.prefix = {0,0};
  }
  // do result
  distance = n.start - mapping.source_range_start;
  if (distance < 0 || distance > mapping.source_range_len) {
    rm.result = {0,0}; // no overlap
  } else {
    rm.result = { mapping.dest_range_start + distance, std::min(n.length, mapping.source_range_len) };
    // if there's some suffix it will begin at the end of the mapping source range, and if there's not
    // the length will be 0 and it doesn't matter where we set it to
    n.start = mapping.source_range_start + mapping.source_range_len;
    n.length -= rm.result.length;
  }
  // do suffix
  if (n.length == 0) {
    rm.suffix.start = 0;
    rm.suffix.length = 0;
  } else {
    rm.suffix.start = n.start;
    rm.suffix.length = n.length;
  }
  return rm;
}

mapping parseMappingLine(std::string_view line) {
  mapping ret;
  ret.dest_range_start = utils::parseInt(line);
  utils::eatChar(' ', line);
  ret.source_range_start = utils::parseInt(line);
  utils::eatChar(' ', line);
  ret.source_range_len = utils::parseInt(line);

  return ret;
}

std::vector<int64_t> parseSeeds(std::string_view line) {
  std::vector<int64_t> result;
  utils::eatLiteral("seeds:", line);
  while (!line.empty()) {
    utils::eatChar(' ', line);
    result.push_back(utils::parseInt(line));
  }
  return result;
}
std::vector<range> parseSeedRanges(std::string_view line) {
  std::vector<range> result;
  utils::eatLiteral("seeds:", line);
  while (!line.empty()) {
    utils::eatChar(' ', line);
    auto start = utils::parseInt(line);
    utils::eatChar(' ', line);
    auto length = utils::parseInt(line);
    result.push_back({start, length});
  }
  return result;
}

template<bool Debug>
class almanac_runner {
  public:
    almanac_runner() = default;

    void setInitialNumbers(numbers ns) { next_ = std::move(ns); }
    numbers getFinalNumbers() { finishMapping(); return current_; }

    void feedLine(std::string_view line) {
      if (currentMap_ == "") {
        utils::AssertEq(line.empty(), false);
        utils::AssertEq(std::isdigit(line.front()), 0);
        currentMap_ = line;
      } else if (line == "") {
        finishMapping();
        currentMap_ = "";
      } else {
        auto m = parseMappingLine(line);
        runMappingRule(m);
      }
    };
  private:
    // Finishes a "round" of mapping, putting all values back into current_ ready for the next
    void finishMapping() {
      // any numbers which didn't match are reused
      // just need to add the ones we updated back to current_
      if constexpr (Debug) {
        if (!current_.empty())
          fmt::println("Rule {} left {} unchanged!", currentMap_, fmt::join(current_, ", "));
      }
      current_.insert(current_.end(),  next_.begin(), next_.end());
      next_.clear();
    }

    void runMappingRule(mapping m) {
      for (auto it = current_.begin(); it != current_.end(); ) {
        if (auto nv = applyMapping(*it, m)) {
          if constexpr (Debug) {
            fmt::println("Rule {} mapped {} -> {}", currentMap_, *it, *nv);
          }
          next_.push_back(nv.value());
          it = current_.erase(it);
        } else ++it;
      }
    }

    std::string currentMap_ = "seeds";
    numbers current_;
    numbers next_;
};

template<bool Debug>
class almanac_range_runner {
  public:
    almanac_range_runner() = default;

    void setInitialNumbers(number_ranges ns) {
      next_ = std::move(ns);
      if constexpr (Debug) fmt::println("seeds: {}", fmt::join(next_, ", "));
    }
    number_ranges getFinalNumbers() { finishMapping(); return current_; }

    void feedLine(std::string_view line) {
      if (currentMap_ == "") {
        utils::AssertEq(line.empty(), false);
        utils::AssertEq(std::isdigit(line.front()), 0);
        currentMap_ = line;
      } else if (line == "") {
        finishMapping();
        currentMap_ = "";
      } else {
        auto m = parseMappingLine(line);
        runMappingRule(m);
      }
    };
  private:
    // Finishes a "round" of mapping, putting all values back into current_ ready for the next
    void finishMapping() {
      // any numbers which didn't match are reused
      // just need to add the ones we updated back to current_
      if constexpr (Debug) {
        if (!current_.empty())
          fmt::println("Rule {} left {} unchanged!", currentMap_, fmt::join(current_, ", "));
      }
      current_.insert(current_.end(),  next_.begin(), next_.end());
      next_.clear();
    }

    void runMappingRule(const mapping m) {
      for (auto idx = 0; idx < current_.size(); ) {
        auto mres = applyMapping(current_[idx], m);
        if (mres.result.length != 0)
        {
          if constexpr (Debug) {
            fmt::println("Rule {} mapped {} -> {} (pre={} suf={})", currentMap_, current_[idx], mres.result, mres.prefix, mres.suffix);
          }
          next_.push_back(mres.result);
          current_.erase(current_.begin() + idx);
          if (mres.prefix.length != 0) {
            current_.push_back(mres.prefix);
          }
          if (mres.suffix.length != 0) {
            current_.push_back(mres.suffix);
          }
        } else ++idx;
      }
    }

    std::string currentMap_ = "seeds";
    number_ranges current_;
    number_ranges next_;
};

void test() {
  utils::AssertEq(applyMapping(79, {52, 50, 48}).value(), 81l);
  utils::AssertEq(applyMapping(2, {52, 50, 48}).has_value(), false);

  auto sr = parseSeedRanges("seeds: 79 14 55 13");
  utils::AssertEq(sr.size(), 2ul);
  utils::AssertEq(sr[0].start, 79l);
  utils::AssertEq(sr[0].length, 14l);
  utils::AssertEq(sr[1].start, 55l);
  utils::AssertEq(sr[1].length, 13l);
}

int main(int argc, char **argv) {

  test();
  // utils::LineReader lr{"inp/day5_test.txt"};
  utils::LineReader lr{"inp/day5.txt"};
  auto seedLine = *lr.getLine();
  auto seeds = parseSeeds(seedLine);
  auto seedRanges = parseSeedRanges(seedLine);

  almanac_runner<false> alm;
  almanac_range_runner<false> alm_range;

  alm.setInitialNumbers(seeds);
  alm_range.setInitialNumbers(seedRanges);
  while (auto line = lr.getLine()) {
    // fmt::println("{}", *line);
    alm.feedLine(*line);
    alm_range.feedLine(*line);
  }
  {
  auto final1 = alm.getFinalNumbers();
  auto p1 = *std::min_element(final1.begin(), final1.end());
  fmt::println("Day5: Part 1: {}", p1);
  }
  auto final2 = alm_range.getFinalNumbers();
  auto p2 = *std::min_element(final2.begin(), final2.end(), [](const range& l, const range& r) { return l.start < r.start; });
  fmt::println("Day5: Part 2: {}", p2.start);
}
