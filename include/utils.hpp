#include <string_view>
#include <charconv>
#include <stdexcept>
#include <fmt/core.h>
#include <array>

namespace utils {
  template<auto F>
    struct scope_guard {
      ~scope_guard() { F(); };
    };

  inline bool eatChar(const char expected, std::string_view& inp) {
    if (inp.empty())
      throw std::invalid_argument(fmt::format("eatChar expected {} but no input remained", expected));
    if (inp.front() != expected)
      throw std::invalid_argument(fmt::format("char did not match, expected {} got {}", expected, inp.front()));

    inp.remove_prefix(1);

    return true;
  }

  inline bool eatLiteral(const std::string_view prefix, std::string_view& inp) {
    if (inp.starts_with(prefix)) {
      inp.remove_prefix(prefix.size());
      return true;
    }
    return false;
  }

  inline int32_t parseInt(std::string_view& inp) {
    int32_t result;
    auto [ptr, ec] = std::from_chars(inp.data(), inp.data() + inp.size(), result);
    if (ec != std::errc()) throw std::invalid_argument("integer could not be parsed from value");
    inp.remove_prefix(ptr - inp.data());

    return result;
  }

  template<typename T>
  void AssertEq(const T& x, const T& y) {
    if (x != y) {
      fmt::println("ASSERTION FAILED: {} != {}", x, y);
      std::terminate();
    }
  }

}
