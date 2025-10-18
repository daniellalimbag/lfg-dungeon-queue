 #ifndef LFG_UTILS_H
#define LFG_UTILS_H

#include <chrono>
#include <string>

namespace Utils {
  std::chrono::seconds randomDuration(int t1, int t2);

  // prints a complete line (appends a newline) serialized across threads.
  void logln(const std::string& line);

  // prints/updates a single status line (no newline), overwriting previous status.
  void status(const std::string& line);

  // clears the status line (e.g., before printing a block of normal output).
  void clearStatus();
}

#endif
