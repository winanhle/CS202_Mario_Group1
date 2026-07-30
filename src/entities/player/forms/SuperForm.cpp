#include "SuperForm.h"
#include "NormalForm.h"
#include "FireForm.h"

std::unique_ptr<IPlayerForm> SuperForm::takeDamage() const {
    return std::make_unique<NormalForm>();
}

std::unique_ptr<IPlayerForm> SuperForm::evolve(PowerUpType type) const {
    if (type == PowerUpType::FireFlower)
        return std::make_unique<FireForm>();
    return nullptr;
}
