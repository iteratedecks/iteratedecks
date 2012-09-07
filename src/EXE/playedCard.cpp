#include "playedCard.hpp"
#include <sstream>
#include <iomanip>

bool PlayedCard::canAttack() const {
    if(this->GetEffect(DMGDEPENDANT_IMMOBILIZE) > 0) {
        return false;
    } else if (this->GetEffect(ACTIVATION_JAM) > 0) {
        return false;
    } else if (this->GetEffect(ACTIVATION_FREEZE) > 0) {
        return false;
    } else if (this->GetAttack() <= 0) {
        return false;
    }
    return true;
}

unsigned int PlayedCard::getUniqueId() const {
    return this->uniqueId;
}

unsigned int const PlayedCard::numberOfCardLines(6);

std::string PlayedCard::toRectString(unsigned int const w, unsigned int const i) const
{
    std::stringstream ssLine;
    switch(i) {
        case 0: case 5: {
            ssLine << "+" << std::setw(w-2);
            ssLine << std::setfill('-') << "" << "+";
        } break;
        case 1: {
            ssLine << "|";
            ssLine << std::setw(4) << this->getUniqueId();
            ssLine << std::setw(w-4-2-2) << "";
            if (this->GetWait() > 0) {
                ssLine << std::setw(2) << (unsigned int)this->GetWait();
            } else {
                ssLine << "  ";
            }
            ssLine << "|";
        } break;
        case 2: {
            ssLine << "|" << std::setw(w-2) << std::left << this->GetName() << "|";
        } break;
        case 3: {
            ssLine << "|";
            int l = w-2;
            if(!this->IsAlive()) {
                ssLine << "Dead ";
                l-=5;
            }
            if(this->IsDiseased()) {
                ssLine << "D ";
                l-=2;
            }
            if(this->GetEffect(DMGDEPENDANT_POISON)) {
                ssLine << "Po" << std::setw(2) << (UINT)this->GetEffect(DMGDEPENDANT_POISON) << " ";
                l-=2+2+1;
            }
            ssLine << std::setw(l) << "";
            ssLine << "|";
        } break;
        case 4: {
            ssLine << "|";
            ssLine << std::setw(w-2-5) << "";
            ssLine << std::setw(2) << (unsigned int)this->GetAttack();
            ssLine << "/";
            ssLine << std::setw(2) << (unsigned int)this->GetHealth();
            ssLine << "|";
        } break;
    }
    return ssLine.str();
}
