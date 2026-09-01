#include "SoundManager.h"
#include <stdexcept>
#include <vector>

SoundManager::SoundManager()
{
    // =========================
    // GAME SOUNDS
    // =========================

    if (!m_bumpBuffer.loadFromFile("assets/sound/ui/bump.wav"))
        throw std::runtime_error("Failed to load ui/bump.wav");

    if (!m_brickBuffer.loadFromFile("assets/sound/brick.wav"))
        throw std::runtime_error("Failed to load brick.wav");

    if (!m_coinBuffer.loadFromFile("assets/sound/coin.wav"))
        throw std::runtime_error("Failed to load coin.wav");

    if (!m_fireballBuffer.loadFromFile("assets/sound/fireball.wav"))
        throw std::runtime_error("Failed to load fireball.wav");

    if (!m_flagpoleBuffer.loadFromFile("assets/sound/flagpole.wav"))
        throw std::runtime_error("Failed to load flagpole.wav");

    if (!m_itemBuffer.loadFromFile("assets/sound/item.wav"))
        throw std::runtime_error("Failed to load item.wav");

    if (!m_jumpBuffer.loadFromFile("assets/sound/jump.wav"))
        throw std::runtime_error("Failed to load jump.wav");

    if (!m_jumpSmallBuffer.loadFromFile("assets/sound/jumpsmall.wav"))
        throw std::runtime_error("Failed to load jumpsmall.wav");

    if (!m_powerupBuffer.loadFromFile("assets/sound/powerup.wav"))
        throw std::runtime_error("Failed to load powerup.wav");

    if (!m_stompBuffer.loadFromFile("assets/sound/stomp.wav"))
        throw std::runtime_error("Failed to load stomp.wav");


    // =========================
    // UI SOUNDS
    // =========================

    if (!m_uiBumpBuffer.loadFromFile("assets/sound/ui/bump.wav"))
        throw std::runtime_error("Failed to load ui/bump.wav");

    if (!m_pauseBuffer.loadFromFile("assets/sound/ui/pause.wav"))
        throw std::runtime_error("Failed to load ui/pause.wav");

    if (!m_saveGameBuffer.loadFromFile("assets/sound/ui/save_game.wav"))
        throw std::runtime_error("Failed to load ui/save_game.wav");

    if (!m_selectSuccessBuffer.loadFromFile("assets/sound/ui/select_success.wav"))
        throw std::runtime_error("Failed to load ui/select_success.wav");

    if (!m_selectBuffer.loadFromFile("assets/sound/ui/select.wav"))
        throw std::runtime_error("Failed to load ui/select.wav");

    if (!m_stageClearBuffer.loadFromFile("assets/sound/ui/stage_clear.wav"))
        throw std::runtime_error("Failed to load ui/stage_clear.wav");

    if (!m_worldClearBuffer.loadFromFile("assets/sound/ui/world_clear.wav"))
        throw std::runtime_error("Failed to load ui/world_clear.wav");


    // =========================
    // CREATE SOUND OBJECTS
    // =========================

    m_bumpSound = std::make_unique<sf::Sound>(m_bumpBuffer);
    m_brickSound = std::make_unique<sf::Sound>(m_brickBuffer);
    m_coinSound = std::make_unique<sf::Sound>(m_coinBuffer);
    m_fireballSound = std::make_unique<sf::Sound>(m_fireballBuffer);
    m_flagpoleSound = std::make_unique<sf::Sound>(m_flagpoleBuffer);
    m_itemSound = std::make_unique<sf::Sound>(m_itemBuffer);
    m_jumpSound = std::make_unique<sf::Sound>(m_jumpBuffer);
    m_jumpSmallSound = std::make_unique<sf::Sound>(m_jumpSmallBuffer);
    m_powerupSound = std::make_unique<sf::Sound>(m_powerupBuffer);
    m_stompSound = std::make_unique<sf::Sound>(m_stompBuffer);

    m_uiBumpSound = std::make_unique<sf::Sound>(m_uiBumpBuffer);
    m_pauseSound = std::make_unique<sf::Sound>(m_pauseBuffer);
    m_saveGameSound = std::make_unique<sf::Sound>(m_saveGameBuffer);
    m_selectSuccessSound = std::make_unique<sf::Sound>(m_selectSuccessBuffer);
    m_selectSound = std::make_unique<sf::Sound>(m_selectBuffer);
    m_stageClearSound = std::make_unique<sf::Sound>(m_stageClearBuffer);
    m_worldClearSound = std::make_unique<sf::Sound>(m_worldClearBuffer);

    // Apply initial volume
    setVolume(m_volume);
}


// ============================================================
// GAME SOUNDS
// ============================================================

void SoundManager::playBump()
{
    m_bumpSound->play();
}

void SoundManager::playBrick()
{
    m_brickSound->play();
}

void SoundManager::playCoin()
{
    m_coinSound->play();
}

void SoundManager::playFireball()
{
    m_fireballSound->play();
}

void SoundManager::playFlagpole()
{
    m_flagpoleSound->play();
}

void SoundManager::playItem()
{
    m_itemSound->play();
}

void SoundManager::playJump()
{
    m_jumpSound->play();
}

void SoundManager::playJumpSmall()
{
    m_jumpSmallSound->play();
}

void SoundManager::playPowerUp()
{
    m_powerupSound->play();
}

void SoundManager::playStomp()
{
    m_stompSound->play();
}


// ============================================================
// UI SOUNDS
// ============================================================

void SoundManager::playUIBump()
{
    m_uiBumpSound->play();
}

void SoundManager::playPause()
{
    m_pauseSound->play();
}

void SoundManager::playSaveGame()
{
    m_saveGameSound->play();
}

void SoundManager::playSelectSuccess()
{
    m_selectSuccessSound->play();
}

void SoundManager::playSelect()
{
    m_selectSound->play();
}

void SoundManager::playStageClear()
{
    m_stageClearSound->play();
}

void SoundManager::playWorldClear()
{
    m_worldClearSound->play();
}


// ============================================================
// VOLUME
// ============================================================

void SoundManager::setVolume(float volume)
{
    // Keep volume in valid SFML range.
    if (volume < 0.f)
        volume = 0.f;

    if (volume > 100.f)
        volume = 100.f;

    m_volume = volume;

    // Game sounds
    m_bumpSound->setVolume(m_volume);
    m_brickSound->setVolume(m_volume);
    m_coinSound->setVolume(m_volume);
    m_fireballSound->setVolume(m_volume);
    m_flagpoleSound->setVolume(m_volume);
    m_itemSound->setVolume(m_volume);
    m_jumpSound->setVolume(m_volume);
    m_jumpSmallSound->setVolume(m_volume);
    m_powerupSound->setVolume(m_volume);
    m_stompSound->setVolume(m_volume);

    // UI sounds
    m_uiBumpSound->setVolume(m_volume);
    m_pauseSound->setVolume(m_volume);
    m_saveGameSound->setVolume(m_volume);
    m_selectSuccessSound->setVolume(m_volume);
    m_selectSound->setVolume(m_volume);
    m_stageClearSound->setVolume(m_volume);
    m_worldClearSound->setVolume(m_volume);
}

float SoundManager::getVolume() const
{
    return m_volume;
}


// ============================================================
// STOP ALL SOUNDS
// ============================================================

void SoundManager::stopAll()
{
    m_bumpSound->stop();
    m_brickSound->stop();
    m_coinSound->stop();
    m_fireballSound->stop();
    m_flagpoleSound->stop();
    m_itemSound->stop();
    m_jumpSound->stop();
    m_jumpSmallSound->stop();
    m_powerupSound->stop();
    m_stompSound->stop();

    m_uiBumpSound->stop();
    m_pauseSound->stop();
    m_saveGameSound->stop();
    m_selectSuccessSound->stop();
    m_selectSound->stop();
    m_stageClearSound->stop();
    m_worldClearSound->stop();
}