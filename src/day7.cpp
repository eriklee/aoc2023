#include "utils.hpp"

#include <algorithm>
#include <fmt/format.h>
#include <vector>
#include <string>
#include <string_view>

enum class HandType : uint8_t {
  UNKNOWN = 0,
  HIGH_CARD,
  ONE_PAIR,
  TWO_PAIR,
  THREE_OF_A_KIND,
  FULL_HOUSE,
  FOUR_OF_A_KIND,
  FIVE_OF_A_KIND,
};

template <> struct fmt::formatter<HandType> {
  constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
    return ctx.end();
  }
  auto format(const HandType& ht, format_context& ctx) const -> format_context::iterator {
    if (ht == HandType::UNKNOWN) return fmt::format_to(ctx.out(), "{}", "UNKNOWN");
    if (ht == HandType::HIGH_CARD) return fmt::format_to(ctx.out(), "{}", "HIGH_CARD");
    if (ht == HandType::ONE_PAIR) return fmt::format_to(ctx.out(), "{}", "ONE_PAIR");
    if (ht == HandType::TWO_PAIR) return fmt::format_to(ctx.out(), "{}", "TWO_PAIR");
    if (ht == HandType::THREE_OF_A_KIND) return fmt::format_to(ctx.out(), "{}", "THREE_OF_A_KIND");
    if (ht == HandType::FULL_HOUSE) return fmt::format_to(ctx.out(), "{}", "FULL_HOUSE");
    if (ht == HandType::FOUR_OF_A_KIND) return fmt::format_to(ctx.out(), "{}", "FOUR_OF_A_KIND");
    if (ht == HandType::FIVE_OF_A_KIND) return fmt::format_to(ctx.out(), "{}", "FIVE_OF_A_KIND");
    __builtin_unreachable();
  }
};

using card_t = uint8_t;
using counts_t = std::array<uint8_t, 15>;

struct Hand {
  HandType type = HandType::UNKNOWN;
  std::array<card_t, 5> hand;
  uint64_t bid;

  // Fields above are ordered so the default ordering is correct
  auto operator<=>(const Hand& other) const = default;
};
template <> struct fmt::formatter<Hand> {
  constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
    return ctx.end();
  }
  auto format(const Hand& h, format_context& ctx) const -> format_context::iterator {
    return fmt::format_to(ctx.out(), 
        "Hand: {} type={} bid={}", fmt::join(h.hand.begin(), h.hand.end(), ","), h.type, h.bid);
  }
};

// maps a card char to a uint8_t [2,14]
card_t parseCard(const char c) {
  switch (c) {
    case 'A': return 14;
    case 'K': return 13;
    case 'Q': return 12;
    case 'J': return 11;
    case 'T': return 10;
    default: return (c - '0');
  }
}

counts_t buildCounts(const std::array<card_t, 5>& hand) {
  counts_t ret{};
  for (const auto c : hand) {
    ++ret[c];
  }
  return ret;
}

HandType getHandType(const std::array<card_t, 5>& hand) {
  auto counts = buildCounts(hand);

  if (std::any_of(counts.begin(), counts.end(), [](const auto c){ return c == 5; }))
    return HandType::FIVE_OF_A_KIND;
  if (std::any_of(counts.begin(), counts.end(), [](const auto c){ return c == 4; }))
    return HandType::FOUR_OF_A_KIND;
  if (std::any_of(counts.begin(), counts.end(), [](const auto c){ return c == 3; })) {
    if (std::any_of(counts.begin(), counts.end(), [](const auto c){ return c == 2; }))
      return HandType::FULL_HOUSE;
    else return HandType::THREE_OF_A_KIND;
  }
  {
    int pairs = 0;
    for (const auto c : counts) { if (c == 2) ++pairs; }
    if (pairs == 2) return HandType::TWO_PAIR;
    else if (pairs == 1) return HandType::ONE_PAIR;
  }
  // We could assume this is the default, but let's just double check...
  if (std::all_of(counts.begin(), counts.end(), [](const auto c) {return c <= 1; })) return HandType::HIGH_CARD;

  __builtin_unreachable();
}

HandType getHandTypeJokers(const std::array<card_t, 5>& hand) {
  auto counts = buildCounts(hand);
  auto jokerCount = counts[1];

  if (std::any_of(counts.begin(), counts.end(), [](const auto c){ return c == 5; }))
    return HandType::FIVE_OF_A_KIND;
  if (std::any_of(counts.begin(), counts.end(), [](const auto c){ return c == 4; })) {
    if (jokerCount == 1) return HandType::FIVE_OF_A_KIND;
    if (jokerCount == 4) return HandType::FIVE_OF_A_KIND;
    return HandType::FOUR_OF_A_KIND;
  }
  if (std::any_of(counts.begin(), counts.end(), [](const auto c){ return c == 3; })) {
    if (jokerCount == 2) return HandType::FIVE_OF_A_KIND;
    if (jokerCount == 1) return HandType::FOUR_OF_A_KIND;

    if (std::any_of(counts.begin(), counts.end(), [](const auto c){ return c == 2; })) {
      if (jokerCount == 3) return HandType::FIVE_OF_A_KIND;
      else return HandType::FULL_HOUSE;
    }
    else {
      if (jokerCount == 3) return HandType::FOUR_OF_A_KIND;
      else return HandType::THREE_OF_A_KIND;
    }
  }
  // We now have no cards with 3 or more copies
  int pairs = 0;
  for (const auto c : counts) { if (c == 2) ++pairs; }
  // If there are 2 pairs 
  if (pairs == 2) {
    if (jokerCount == 2) return HandType::FOUR_OF_A_KIND;
    if (jokerCount == 1) return HandType::FULL_HOUSE;
    return HandType::TWO_PAIR;
  }
  // 1 pair, 3 singles
  else if (pairs == 1) {
    if (jokerCount == 2) return HandType::THREE_OF_A_KIND;
    if (jokerCount == 1) return HandType::THREE_OF_A_KIND;
    return HandType::ONE_PAIR;
  }

  if (pairs == 0) return jokerCount == 1 ? HandType::ONE_PAIR : HandType::HIGH_CARD;
  __builtin_unreachable();
}

HandType getHandTypeJokers(const HandType oldType, const std::array<card_t, 5>& hand) {
  auto jokerCount = 0;
  for (const auto c : hand) { if (c == 1) jokerCount++; }

  if (jokerCount == 0) return oldType;

  switch (oldType) {
    case HandType::FIVE_OF_A_KIND: return HandType::FIVE_OF_A_KIND;
                                   // must be jc == 5;
    case HandType::FOUR_OF_A_KIND: return HandType::FIVE_OF_A_KIND;
                                   // must be either jc == 1 or 4, result is the same
    case HandType::FULL_HOUSE: return HandType::FIVE_OF_A_KIND;
                               // one of them must have been jokers, so they're all the same
    case HandType::THREE_OF_A_KIND: return HandType::FOUR_OF_A_KIND;
    case HandType::TWO_PAIR: return jokerCount == 2 ? HandType::FOUR_OF_A_KIND : HandType::FULL_HOUSE;
                             // either 2 jokers + another pair, or two pairs + 1 joker
    case HandType::ONE_PAIR: return HandType::THREE_OF_A_KIND;
                             // either 1 or 2 jokers
    case HandType::HIGH_CARD: return jokerCount == 1 ? HandType::ONE_PAIR : HandType::HIGH_CARD;
    case HandType::UNKNOWN: __builtin_unreachable();
    }
  __builtin_unreachable();
}

Hand parseHand(std::string_view l) {
  Hand result;
  for (int i = 0; i < 5; ++i) {
    result.hand[i] = parseCard(l[i]);
  }
  l.remove_prefix(6); // 5 cards + space
  result.bid = utils::parseInt(l);
  result.type = getHandType(result.hand);
  return result;
}

void jacksToJokers(Hand& h) {
  // First turn all jacks into jokers
  for (auto& c : h.hand) { if (c == 11) c = 1; }
  // Recalculate the type
  auto oldType = h.type;
  h.type = getHandTypeJokers(oldType, h.hand);
  /*
  if (std::none_of(h.hand.begin(), h.hand.end(), [](const auto c) { return c == 1; })) {
    utils::Assert(h.type == oldType);
  } else {
    fmt::println("jokerfied: {}, updated {} -> {}", h, oldType, h.type);
    utils::Assert(h.type == getHandTypeJokers(h.hand));
  }
  */
}

uint64_t calculateTotalWinnings(const std::vector<Hand>& hands) {
  uint64_t result = 0;
  for (int i = 0; i < hands.size(); ++i) {
    result += (i + 1) * hands[i].bid;
  }
  return result;
}

void test() {
  auto raw = "32T3K 765";
  auto th = parseHand(raw);
  utils::Assert(th.type == HandType::ONE_PAIR);
  utils::AssertEq(th.bid, 765ul);

  raw = "JJ8JJ 765";
  th = parseHand(raw);
  utils::Assert(th.type == HandType::FOUR_OF_A_KIND);
  jacksToJokers(th);
  utils::Assert(th.type == HandType::FIVE_OF_A_KIND);
}

int main(int argc, char **argv) {
  test();
  // utils::LineReader lr{"inp/day7_test.txt"};
  utils::LineReader lr{"inp/day7.txt"};

  std::vector<Hand> hands;
  while (auto line = lr.getLine()) {
    hands.push_back(parseHand(*line));
  }
  std::sort(hands.begin(), hands.end());
  uint64_t part1 = calculateTotalWinnings(hands);
  fmt::println("Day7: Part 1: {}", part1);

  for (auto& h : hands) { jacksToJokers(h); }
  std::sort(hands.begin(), hands.end());
  uint64_t part2 = calculateTotalWinnings(hands);
  
  fmt::println("Day7: Part 2: {}", part2);
}
