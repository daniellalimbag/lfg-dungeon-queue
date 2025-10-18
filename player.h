 #ifndef LFG_PLAYER_H
 #define LFG_PLAYER_H
 
 #include <string>
 #include <cstdint>
 
 // player roles available in the LFG system
 // keep in sync with any logic that counts roles in parties
 enum class PlayerRole : std::uint8_t {
   TANK = 0,
   HEALER = 1,
   DPS = 2
 };
 
 class Player {
 public:
   Player(int id, PlayerRole role);
   ~Player();

   int getId() const;
   PlayerRole getRole() const;
   std::string roleString() const;
 
 private:
   int id_;
   PlayerRole role_;
 };
 
 #endif
