 #include "utils.h"

#include <random>
#include <mutex>
#include <iostream>
#include <string>

namespace Utils {
  std::chrono::seconds randomDuration(int t1, int t2) {
    if (t1 > t2) std::swap(t1, t2);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(t1, t2);
    return std::chrono::seconds{dist(gen)};
  }

  namespace {
    std::mutex& logMutex() {
      static std::mutex m; return m;
    }
    size_t& statusLen() {
      static size_t len = 0; return len;
    }
    bool& hasStatus() {
      static bool on = false; return on;
    }
  }

  void clearStatus() {
    std::lock_guard<std::mutex> lk(logMutex());
    if (statusLen() > 0) {
      std::cout << "\r" << std::string(statusLen(), ' ') << "\r" << std::flush;
      statusLen() = 0;
    }
    hasStatus() = false;
  }

  void status(const std::string& line) {
    std::lock_guard<std::mutex> lk(logMutex());
    // erase previous
    if (statusLen() > 0) {
      std::cout << "\r" << std::string(statusLen(), ' ') << "\r";
    }
    std::cout << line << std::flush;
    statusLen() = line.size();
    hasStatus() = true;
  }

  void logln(const std::string& line) {
    std::lock_guard<std::mutex> lk(logMutex());
    // temporarily clear status line if present
    if (statusLen() > 0) {
      std::cout << "\r" << std::string(statusLen(), ' ') << "\r";
    }
    std::cout << line << "\n";
    // restore status line if previously set
    if (hasStatus()) {
      std::cout << std::flush;
      std::cout << std::flush;
      std::cout << std::flush;
      std::cout << std::flush;
      std::cout << std::flush;
    }
    if (hasStatus() && statusLen() > 0) {
      // no stored content; caller will re-render on next tick. Just ensure cursor at start.
      std::cout << std::flush;
    }
  }
}
