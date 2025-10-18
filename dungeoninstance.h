 #ifndef LFG_DUNGEONINSTANCE_H
#define LFG_DUNGEONINSTANCE_H

#include <atomic>
#include <thread>
#include <mutex>
#include <optional>
#include <functional>
#include <chrono>

#include "party.h"

struct InstanceSummary {
  int instanceId {0};
  int partiesServed {0};
  int totalTimeSec {0};
};

class DungeonInstance {
public:
  explicit DungeonInstance(int id);
  ~DungeonInstance();

  void startRun(const Party& party, std::chrono::seconds duration);
  bool isActive() const;
  void join();
  InstanceSummary summary() const;

  // set a callback invoked when a run completes
  void setOnFinish(std::function<void(int)> cb);

private:
  mutable std::mutex mtx_;

  int id_ {0};
  std::atomic<bool> active_ {false};
  std::thread worker_;

  std::function<void(int)> onFinish_;

  int partiesServed_ {0};
  int totalTimeSec_ {0};
};

#endif
