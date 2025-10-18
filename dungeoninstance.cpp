 #include "dungeoninstance.h"
#include <iostream>
#include <string>
#include "utils.h"

using namespace std::chrono_literals;

DungeonInstance::DungeonInstance(int id) : id_(id) {}

DungeonInstance::~DungeonInstance() {
  join();
}

void DungeonInstance::startRun(const Party&, std::chrono::seconds duration) {
  // move out any previous finished worker to join it without holding the mutex
  std::thread toJoin;
  {
    std::scoped_lock lock(mtx_);
    if (worker_.joinable() && !active_.load()) {
      toJoin = std::move(worker_);
    }
  }
  if (toJoin.joinable()) toJoin.join();

  std::scoped_lock lock(mtx_);
  if (active_) return;
  active_ = true;
  int id = id_;
  worker_ = std::thread([this, duration, id]() {
    Utils::logln(std::string("Instance ") + std::to_string(id) + ": start, duration_s=" + std::to_string(duration.count()));
    std::this_thread::sleep_for(duration);
    totalTimeSec_ += static_cast<int>(duration.count());
    partiesServed_ += 1;
    active_ = false;
    Utils::logln(std::string("Instance ") + std::to_string(id) + ": finish, duration_s=" + std::to_string(duration.count()));
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
