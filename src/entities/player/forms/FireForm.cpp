#include "FireForm.h"
#include "SuperForm.h"

std::unique_ptr<IPlayerForm> FireForm::takeDamage() const {
    return std::make_unique<SuperForm>();
}

std::unique_ptr<IPlayerForm> FireForm::evolve(PowerUpType type) const {
    return nullptr;
}
