#include "SoundManager.h"
#include <iostream>
#include <vector>

SoundManager::SoundManager()
{
    auto loadBuffer = [](sf::SoundBuffer& buf, const std::string& path) -> bool {
        if (!buf.loadFromFile(path)) {
            std::cerr << "[SoundManager] WARNING: Failed to load sound: " << path << std::endl;
            return false;
        }
        return true;
    };

    // =========================
    // GAME SOUNDS
    // =========================

    if (!loadBuffer(m_bumpBuffer, "assets/sound/ui/bump.wav"))
        loadBuffer(m_bumpBuffer, "assets/sound/bump.wav");

    loadBuffer(m_brickBuffer, "assets/sound/brick.wav");
    loadBuffer(m_coinBuffer, "assets/sound/coin.wav");
    loadBuffer(m_fireballBuffer, "assets/sound/fireball.wav");
    loadBuffer(m_flagpoleBuffer, "assets/sound/flagpole.wav");
    loadBuffer(m_itemBuffer, "assets/sound/item.wav");
    loadBuffer(m_jumpBuffer, "assets/sound/jump.wav");
    loadBuffer(m_jumpSmallBuffer, "assets/sound/jumpsmall.wav");
    loadBuffer(m_powerupBuffer, "assets/sound/powerup.wav");
    loadBuffer(m_stompBuffer, "assets/sound/stomp.wav");
    loadBuffer(m_oneUpBuffer, "assets/sound/1up.wav");

    // =========================
    // UI SOUNDS
    // =========================

    loadBuffer(m_uiBumpBuffer, "assets/sound/ui/bump.wav");
    loadBuffer(m_pauseBuffer, "assets/sound/ui/pause.wav");
    loadBuffer(m_saveGameBuffer, "assets/sound/ui/save_game.wav");
    loadBuffer(m_selectSuccessBuffer, "assets/sound/ui/select_success.wav");
    loadBuffer(m_selectBuffer, "assets/sound/ui/select.wav");
    loadBuffer(m_stageClearBuffer, "assets/sound/ui/stage_clear.wav");
    loadBuffer(m_worldClearBuffer, "assets/sound/ui/world_clear.wav");
    loadBuffer(m_gameOverBuffer, "assets/sound/game_over.wav");


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
    m_oneUpSound = std::make_unique<sf::Sound>(m_oneUpBuffer);

    m_uiBumpSound = std::make_unique<sf::Sound>(m_uiBumpBuffer);
    m_pauseSound = std::make_unique<sf::Sound>(m_pauseBuffer);
    m_saveGameSound = std::make_unique<sf::Sound>(m_saveGameBuffer);
    m_selectSuccessSound = std::make_unique<sf::Sound>(m_selectSuccessBuffer);
    m_selectSound = std::make_unique<sf::Sound>(m_selectBuffer);
    m_stageClearSound = std::make_unique<sf::Sound>(m_stageClearBuffer);
    m_worldClearSound = std::make_unique<sf::Sound>(m_worldClearBuffer);
    m_gameOverSound = std::make_unique<sf::Sound>(m_gameOverBuffer);

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

void SoundManager::playOneUp()
{
    m_oneUpSound->play();
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
    m_stompSound->play();
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
    if (m_worldClearSound->getStatus() != sf::Sound::Status::Playing)
    {
        m_worldClearSound->play();
    }
}

void SoundManager::playGameOver()
{
    stopPlayMusic();
    m_gameOverSound->play();
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
    m_gameOverSound->setVolume(m_volume);
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
    m_gameOverSound->stop();

    stopMenuMusic();
    stopPlayMusic();
}

void SoundManager::playMenuMusic()
{
    stopPlayMusic();
    if (m_menuMusic.getStatus() == sf::SoundStream::Status::Playing)
        return;

    if (m_menuMusic.openFromFile("assets/sound/menu_music.mp3"))
    {
        m_menuMusic.setLooping(true);
        m_menuMusic.setVolume(m_volume * 0.5f);
        m_menuMusic.play();
    }
}

void SoundManager::stopMenuMusic()
{
    m_menuMusic.stop();
}

void SoundManager::playGroundTheme()
{
    m_menuMusic.stop();
    m_castleTheme.stop();
    if (m_groundTheme.getStatus() == sf::SoundStream::Status::Playing)
        return;

    if (m_groundTheme.openFromFile("assets/sound/ground_theme.mp3"))
    {
        m_groundTheme.setLooping(true);
        m_groundTheme.setVolume(m_volume * 0.5f);
        m_groundTheme.play();
    }
}

void SoundManager::playCastleTheme()
{
    m_menuMusic.stop();
    m_groundTheme.stop();
    if (m_castleTheme.getStatus() == sf::SoundStream::Status::Playing)
        return;

    if (m_castleTheme.openFromFile("assets/sound/castle_theme.mp3"))
    {
        m_castleTheme.setLooping(true);
        m_castleTheme.setVolume(m_volume * 0.5f);
        m_castleTheme.play();
    }
}

void SoundManager::stopPlayMusic()
{
    m_groundTheme.stop();
    m_castleTheme.stop();
}