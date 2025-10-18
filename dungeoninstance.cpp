 #include "dungeoninstance.h"

using namespace std::chrono_literals;

DungeonInstance::DungeonInstance(int id) : id_(id) {}

DungeonInstance::~DungeonInstance() {
  join();
}

void DungeonInstance::startRun(const Party& /*party*/, std::chrono::seconds duration) {
  std::scoped_lock lock(mtx_);
  // TODO: validate no active run; otherwise wait or return error depending on design.
  if (active_) return;
  active_ = true;
  worker_ = std::thread([this, duration]() {
    // TODO: add detailed per-party simulation/logging if needed
    std::this_thread::sleep_for(duration);
    totalTimeSec_ += static_cast<int>(duration.count());
    partiesServed_ += 1;
    active_ = false;
    auto cb = onFinish_;
    if (cb) cb(id_);
  });
}

bool DungeonInstance::isActive() const { return active_.load(); }

void DungeonInstance::join() {
  std::unique_lock lock(mtx_);
  if (worker_.joinable()) {
    auto t = std::move(worker_);
    lock.unlock();
    t.join();
  }
}

InstanceSummary DungeonInstance::summary() const {
  std::scoped_lock lock(mtx_);
  return InstanceSummary{ id_, partiesServed_, totalTimeSec_ };
}

void DungeonInstance::setOnFinish(std::function<void(int)> cb) {
  std::scoped_lock lock(mtx_);
  onFinish_ = std::move(cb);
}
