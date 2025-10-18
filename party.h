 #ifndef LFG_PARTY_H
 #define LFG_PARTY_H
 
 #include <vector>
 #include <mutex>
 
 #include "player.h"
 
 class Party {
 public:
   Party();
   ~Party();
 
   // can't copy party because copying a mutex is not safe or allowed
   // move party while creating a fresh mutex to avoid synchronization issues
   Party(const Party&) = delete;
   Party& operator=(const Party&) = delete;
   Party(Party&& other) noexcept;
   Party& operator=(Party&& other) noexcept;
 
   bool addPlayer(const Player& p);
   bool isComplete() const;
   void clear();
   std::vector<Player> members() const;
   int tankCount() const;
   int healerCount() const;
   int dpsCount() const;
 
 private:
   // NOTE: if party is shared across threads, protect access with this mutex
   // TODO: decide ownership/synchronization model (immutable once formed vs concurrent writes)
   mutable std::mutex mtx_;
 
   std::vector<Player> members_;
 
   int tanks_ {0};
   int healers_ {0};
   int dps_ {0};
 };
 
 #endif