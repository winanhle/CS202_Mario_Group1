#include "Enemy.h"
#include "../../interfaces/IPlayerManager.h"

Enemy::Enemy(float x,
             float y,
             sf::Texture& texture)
    : m_sprite(texture) {
    m_sprite.setPosition({x,y});
    m_sprite.setScale({0.5f, 0.5f});
}

void Enemy::update(float dt) {

}

void Enemy::render(sf::RenderWindow& window) const
{
    if (!m_dead) window.draw(m_sprite);
}

sf::FloatRect Enemy::getHitbox() const
{
    return m_sprite.getGlobalBounds();
}

void Enemy::move(float dt) {
    m_velocity.x = m_moveSpeed * static_cast<float>(m_direction);
    m_sprite.move({m_velocity.x * dt, 0.f});
}

void Enemy::reverseDirection()
{
    m_direction *= -1;
}

void Enemy::onStomp()
{
    m_dead = true;
    m_alive = false;
}