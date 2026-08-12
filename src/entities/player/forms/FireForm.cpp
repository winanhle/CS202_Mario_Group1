#include "FireForm.h"
#include "SuperForm.h"

std::unique_ptr<IPlayerForm> FireForm::takeDamage() const {
    return std::make_unique<SuperForm>(*m_offsets);
}

std::unique_ptr<IPlayerForm> FireForm::evolve(PowerUpType type) const {
    (void)type;
    return nullptr; // FireForm là dạng cao nhất
}
