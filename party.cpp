 #include "party.h"

Party::Party() = default;
Party::~Party() = default;

// move constructor
Party::Party(Party&& other) noexcept {
  std::scoped_lock lock(other.mtx_);
  members_ = std::move(other.members_);
  tanks_ = other.tanks_;
  healers_ = other.healers_;
  dps_ = other.dps_;
  other.tanks_ = other.healers_ = other.dps_ = 0;
}

// move assignment operator
Party& Party::operator=(Party&& other) noexcept {
  if (this != &other) {
    std::scoped_lock lock(other.mtx_);
    members_ = std::move(other.members_);
    tanks_ = other.tanks_;
    healers_ = other.healers_;
    dps_ = other.dps_;
    other.tanks_ = other.healers_ = other.dps_ = 0;
  }
  return *this;
}

bool Party::addPlayer(const Player& p) {
  std::scoped_lock lock(mtx_);
  switch (p.getRole()) {
    case PlayerRole::TANK:
      if (tanks_ >= 1) return false;
      tanks_++; break;
    case PlayerRole::HEALER:
      if (healers_ >= 1) return false;
      healers_++; break;
    case PlayerRole::DPS:
      if (dps_ >= 3) return false;
      dps_++; break;
  }
  members_.push_back(p);
  return true;
}

bool Party::isComplete() const {
  std::scoped_lock lock(mtx_);
  return tanks_ == 1 && healers_ == 1 && dps_ == 3;
}

void Party::clear() {
  std::scoped_lock lock(mtx_);
  members_.clear();
  tanks_ = healers_ = dps_ = 0;
}

std::vector<Player> Party::members() const {
  std::scoped_lock lock(mtx_);
  return members_;
}

int Party::tankCount() const { std::scoped_lock lock(mtx_); return tanks_; }
int Party::healerCount() const { std::scoped_lock lock(mtx_); return healers_; }
int Party::dpsCount() const { std::scoped_lock lock(mtx_); return dps_; }
 
