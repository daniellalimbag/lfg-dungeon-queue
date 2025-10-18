 #ifndef LFG_QUEUEMANAGER_H
 #define LFG_QUEUEMANAGER_H
 
 #include <queue>
 #include <vector>
 #include <memory>
 #include <thread>
 #include <mutex>
 #include <condition_variable>
 #include <atomic>
 #include <chrono>
 
 #include "player.h"
 #include "party.h"
 #include "dungeoninstance.h"
 
 class QueueManager {
 public:
   QueueManager();
   ~QueueManager();
 
   void start(int nInstances);
 
   // enqueue initial players by role counts
   void seedPlayers(int tanks, int healers, int dps);
 
   void setClearTimeRange(int t1Sec, int t2Sec);
 
   // stops matchmaking and waits for workers to finish
   void stop();
 
   // returns a snapshot of instance active states
   std::vector<bool> instanceActivity() const;
 
   // returns summaries of all instances
   std::vector<InstanceSummary> summaries(bool waitForAll = false);
 
 private:
   // TODO: design fair matchmaking to avoid starvation and deadlocks.
   // maybe form complete parties from role queues, assign to first available instance?
   void matchmakingLoop_();
 
   // helpers
   bool canFormParty_() const;
   Party formParty_();
   int nextPlayerId_();
   std::chrono::seconds randomClearTime_() const;
 
   // role queues
   mutable std::mutex mtx_;
   std::queue<Player> tanks_;
   std::queue<Player> healers_;
   std::queue<Player> dps_;
 
   // instances
   std::vector<std::unique_ptr<DungeonInstance>> instances_;
 
   // matchmaking
   std::thread matchmaker_;
   std::condition_variable cv_;
   std::atomic<bool> running_ {false};
 
   // time range
   int t1_ {3};
   int t2_ {10};
 
   // id generator for players
   int nextPlayerIdVal_ {1};
 };
 
 #endif
