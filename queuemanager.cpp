 #include "queuemanager.h"

#include <random>
#include <algorithm>

QueueManager::QueueManager() = default;
QueueManager::~QueueManager() { stop(); }

void QueueManager::start(int nInstances) {
  std::scoped_lock lock(mtx_);
  if (running_) return;
  instances_.clear();
  instances_.reserve(nInstances);
  for (int i = 0; i < nInstances; ++i) {
    auto inst = std::make_unique<DungeonInstance>(i);
    inst->setOnFinish([this](int /*id*/){
      std::unique_lock lk(mtx_);
      cv_.notify_all();
    });
    instances_.push_back(std::move(inst));
  }
  running_ = true;
  matchmaker_ = std::thread(&QueueManager::matchmakingLoop_, this);
}

void QueueManager::seedPlayers(int tanks, int healers, int dps) {
  std::scoped_lock lock(mtx_);
  for (int i = 0; i < tanks; ++i) tanks_.emplace(nextPlayerId_(), PlayerRole::TANK);
  for (int i = 0; i < healers; ++i) healers_.emplace(nextPlayerId_(), PlayerRole::HEALER);
  for (int i = 0; i < dps; ++i) dps_.emplace(nextPlayerId_(), PlayerRole::DPS);
  cv_.notify_all();
}

void QueueManager::setClearTimeRange(int t1Sec, int t2Sec) {
  std::scoped_lock lock(mtx_);
  t1_ = t1Sec; t2_ = t2Sec;
}

void QueueManager::stop() {
  {
    std::scoped_lock lock(mtx_);
    if (!running_) return;
    running_ = false;
    cv_.notify_all();
  }
  if (matchmaker_.joinable()) matchmaker_.join();
  for (auto& inst : instances_) inst->join();
}

std::vector<bool> QueueManager::instanceActivity() const {
  std::scoped_lock lock(mtx_);
  std::vector<bool> v;
  v.reserve(instances_.size());
  for (auto& inst : instances_) v.push_back(inst->isActive());
  return v;
}

std::vector<InstanceSummary> QueueManager::summaries(bool waitForAll) {
  if (waitForAll) {
    for (auto& inst : instances_) inst->join();
  }
  std::scoped_lock lock(mtx_);
  std::vector<InstanceSummary> out;
  out.reserve(instances_.size());
  for (auto& inst : instances_) out.push_back(inst->summary());
  return out;
}

bool QueueManager::canFormParty_() const {
  // TODO: prevent starvation by considering queue ages/priorities.
  return !tanks_.empty() && !healers_.empty() && dps_.size() >= 3;
}

Party QueueManager::formParty_() {
  // precondition: caller holds mtx_ and canFormParty_ is true.
  Party p;
  p.addPlayer(tanks_.front()); tanks_.pop();
  p.addPlayer(healers_.front()); healers_.pop();
  for (int i = 0; i < 3; ++i) { p.addPlayer(dps_.front()); dps_.pop(); }
  return p;
}

int QueueManager::nextPlayerId_() { return nextPlayerIdVal_++; }

std::chrono::seconds QueueManager::randomClearTime_() const {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(t1_, t2_);
  return std::chrono::seconds{dist(gen)};
}

void QueueManager::matchmakingLoop_() {
  while (running_) {
    Party party;
    int freeIdx = -1;
    {
      std::unique_lock lock(mtx_);
      cv_.wait(lock, [this]{
        return !running_ || isDone_unsafe_() || (canFormParty_() && hasFreeInstance_());
      });
      if (!running_) break;
      if (isDone_unsafe_()) break;
      if (!(canFormParty_() && hasFreeInstance_())) continue;
      // form a party and select free instance in round-robin order
      party = formParty_();
      const int n = static_cast<int>(instances_.size());
      if (nextInstanceIdx_ >= n) nextInstanceIdx_ = 0;
      for (int k = 0; k < n; ++k) {
        const int i = (nextInstanceIdx_ + k) % n;
        if (!instances_[i]->isActive()) { freeIdx = i; break; }
      }
      if (freeIdx >= 0) {
        nextInstanceIdx_ = (freeIdx + 1) % n;
      }
    }

    if (freeIdx >= 0) {
      auto dur = randomClearTime_();
      instances_[freeIdx]->startRun(party, dur);
      cv_.notify_all();
    } else {
      // no free instance right now
      // wait for callback to notify
    }
  }
}

bool QueueManager::hasFreeInstance_() const {
  for (auto& inst : instances_) if (!inst->isActive()) return true;
  return false;
}

bool QueueManager::isDone() const {
  std::scoped_lock lock(mtx_);
  return isDone_unsafe_();
}

bool QueueManager::isDone_unsafe_() const {
  bool canForm = !tanks_.empty() && !healers_.empty() && dps_.size() >= 3;
  if (canForm) return false;
  // no party can be formed
  //done only if all instances are idle
  for (auto& inst : instances_) if (inst->isActive()) return false;
  return true;
}
