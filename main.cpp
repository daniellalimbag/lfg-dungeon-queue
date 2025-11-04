 #include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include "queuemanager.h"
#include "utils.h"

static bool parseInt(const std::string& s, int& out) {
  try {
    size_t idx = 0; int v = std::stoi(s, &idx);
    if (idx != s.size()) return false;
    out = v; return true;
  } catch (...) { return false; }
}

int main(int argc, char* argv[]) {
  int n = 0, t = 0, h = 0, d = 0, t1 = 0, t2 = 0;

  if (argc == 7) {
    if (!parseInt(argv[1], n) || !parseInt(argv[2], t) || !parseInt(argv[3], h) ||
        !parseInt(argv[4], d) || !parseInt(argv[5], t1) || !parseInt(argv[6], t2)) {
      std::cerr << "Invalid numeric arguments. Usage: <exe> n t h d t1 t2\n";
      return 1;
    }
  } else {
    std::cout << "Enter n t h d t1 t2: ";
    if (!(std::cin >> n >> t >> h >> d >> t1 >> t2)) {
      std::cerr << "Failed to read inputs.\n"; return 1;
    }
  }
  
  // input validation
  if (n <= 0) { std::cerr << "n must be > 0\n"; return 1; }
  if (t < 0 || h < 0 || d < 0) { std::cerr << "role counts must be >= 0\n"; return 1; }
  if (t1 < 1) { std::cerr << "t1 must be >= 1\n"; return 1; }
  if (t2 < t1 || t2 > 15) { std::cerr << "t2 must be in [t1, 15]\n"; return 1; }

  QueueManager qm;
  qm.setClearTimeRange(t1, t2);
  qm.start(n);
  qm.seedPlayers(t, h, d);
  qm.startFeeder(50, 200, 800);

  using namespace std::chrono_literals;
  while (!qm.isDone()) {
    auto states = qm.instanceActivity();
    std::string line = "Instances: ";
    for (size_t i = 0; i < states.size(); ++i) {
      bool active = states[i];
      line += "[" + std::to_string(i) + ": " + (active ? std::string("active") : std::string("empty")) + "] ";
    }
    Utils::status(line);
    std::this_thread::sleep_for(200ms);
  }
  Utils::clearStatus();
  std::cout << "\n";

  auto summaries = qm.summaries(true);
  std::cout << "Final Summary:\n";
  for (const auto& s : summaries) {
    std::cout << "Dungeon Instance " << s.instanceId
              << ": parties=" << s.partiesServed
              << ", total_time_s=" << s.totalTimeSec << "\n";
  }

  qm.stop();
  return 0;
}
