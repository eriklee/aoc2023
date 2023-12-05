#include <filesystem>
#include <optional>
#include <string_view>
#include <charconv>
#include <stdexcept>
#include <fmt/core.h>
#include <array>

#include <fcntl.h>
#include <sys/mman.h>

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

  inline void eatSpaces(std::string_view& inp) {
    while (!inp.empty() && inp.front() == ' ') inp.remove_prefix(1);
  }

  inline int64_t parseInt(std::string_view& inp) {
    int64_t result;
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

  class LineReader {
    size_t bufidx_ = 0;
    int fd_;
    std::string_view view_;
    void* map_data_;
    size_t size_;

    public:
    LineReader(const std::string& filename) {
      auto full_filename = std::filesystem::current_path().string() + "/" + filename;
      fd_ = open(full_filename.c_str(), O_RDONLY);
      if (fd_ == -1) throw std::runtime_error("invalid filename!");
      size_ = std::filesystem::file_size(filename);
      map_data_ = mmap(0,  size_, PROT_READ, MAP_PRIVATE, fd_, 0);
      if (map_data_ == nullptr) throw std::runtime_error("mmap failed!");
      view_ = std::string_view(reinterpret_cast<char*>(map_data_), size_);
    }
    ~LineReader() { close(fd_); }
    std::optional<std::string_view> getLine() {
      // find next \n
      if (view_.empty()) return std::nullopt;
      auto idx = view_.find('\n');
      auto ret = view_.substr(0, idx);
      view_.remove_prefix(idx + 1); // also remove the newline itself
      return ret;
    }
  };
}
