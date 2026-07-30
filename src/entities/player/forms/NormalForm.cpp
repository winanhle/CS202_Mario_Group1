#include "NormalForm.h"
#include "SuperForm.h"

std::unique_ptr<IPlayerForm> NormalForm::takeDamage() const {
    return nullptr;
}

std::unique_ptr<IPlayerForm> NormalForm::evolve(PowerUpType type) const {
    if (type == PowerUpType::Mushroom)
        return std::make_unique<SuperForm>();
    return nullptr;
}
