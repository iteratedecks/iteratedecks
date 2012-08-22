#include "playedCard.hpp"

bool PlayedCard::canAttack() const {
    if(this->GetEffect(DMGDEPENDANT_IMMOBILIZE) > 0) {
        return false;
    } else if (this->GetEffect(ACTIVATION_JAM) > 0) {
        return false;
    } else if (this->GetEffect(ACTIVATION_FREEZE) > 0) {
        return false;
    }
    return true;
}
