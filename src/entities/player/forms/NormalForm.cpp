#include "NormalForm.h"
#include "SuperForm.h"

std::unique_ptr<IPlayerForm> NormalForm::takeDamage() const {
    return nullptr; // NormalForm là dạng thấp nhất → mất mạng, không demote thêm
}

std::unique_ptr<IPlayerForm> NormalForm::evolve(PowerUpType type) const {
    if (type == PowerUpType::Mushroom)
        return std::make_unique<SuperForm>(*m_offsets);
    return nullptr;
}
