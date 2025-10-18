 #include "player.h"
 
 #include <string>
 
 Player::Player(int id, PlayerRole role) : id_(id), role_(role) {}
 Player::~Player() = default;
 
 int Player::getId() const { return id_; }
 PlayerRole Player::getRole() const { return role_; }
 
 std::string Player::roleString() const {
   switch (role_) {
     case PlayerRole::TANK: return "TANK";
     case PlayerRole::HEALER: return "HEALER";
     case PlayerRole::DPS: return "DPS";
   }
   return "UNKNOWN";
 }
