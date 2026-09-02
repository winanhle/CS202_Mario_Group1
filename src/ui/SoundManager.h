#pragma once

#include "../interfaces/ISoundManager.h"

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <memory>

class SoundManager : public ISoundManager
{
private:

    sf::SoundBuffer m_bumpBuffer;
    sf::SoundBuffer m_brickBuffer;
    sf::SoundBuffer m_coinBuffer;
    sf::SoundBuffer m_deathBuffer;
    sf::SoundBuffer m_fireballBuffer;
    sf::SoundBuffer m_flagpoleBuffer;
    sf::SoundBuffer m_gameOverBuffer;
    sf::SoundBuffer m_itemBuffer;
    sf::SoundBuffer m_jumpBuffer;
    sf::SoundBuffer m_jumpSmallBuffer;
    sf::SoundBuffer m_powerupBuffer;
    sf::SoundBuffer m_stompBuffer;

    sf::SoundBuffer m_uiBumpBuffer;
    sf::SoundBuffer m_pauseBuffer;
    sf::SoundBuffer m_saveGameBuffer;
    sf::SoundBuffer m_selectSuccessBuffer;
    sf::SoundBuffer m_selectBuffer;
    sf::SoundBuffer m_stageClearBuffer;
    sf::SoundBuffer m_worldClearBuffer;

    std::unique_ptr<sf::Sound> m_bumpSound;
    std::unique_ptr<sf::Sound> m_brickSound;
    std::unique_ptr<sf::Sound> m_coinSound;
    std::unique_ptr<sf::Sound> m_deathSound;
    std::unique_ptr<sf::Sound> m_fireballSound;
    std::unique_ptr<sf::Sound> m_flagpoleSound;
    std::unique_ptr<sf::Sound> m_gameOverSound;
    std::unique_ptr<sf::Sound> m_itemSound;
    std::unique_ptr<sf::Sound> m_jumpSound;
    std::unique_ptr<sf::Sound> m_jumpSmallSound;
    std::unique_ptr<sf::Sound> m_powerupSound;
    std::unique_ptr<sf::Sound> m_stompSound;

    // UI
    std::unique_ptr<sf::Sound> m_uiBumpSound;
    std::unique_ptr<sf::Sound> m_pauseSound;
    std::unique_ptr<sf::Sound> m_saveGameSound;
    std::unique_ptr<sf::Sound> m_selectSuccessSound;
    std::unique_ptr<sf::Sound> m_selectSound;
    std::unique_ptr<sf::Sound> m_stageClearSound;
    std::unique_ptr<sf::Sound> m_worldClearSound;

    float m_volume = 100.f;

public:
    SoundManager();
    ~SoundManager() override = default;

    void playBump() override;
    void playBrick() override;
    void playCoin() override;
    void playDeath() override;
    void playFireball() override;
    void playFlagpole() override;
    void playGameOver() override;
    void playItem() override;
    void playJump() override;
    void playJumpSmall() override;
    void playPowerUp() override;
    void playStomp() override;

    void playUIBump() override;
    void playPause() override;
    void playSaveGame() override;
    void playSelectSuccess() override;
    void playSelect() override;
    void playStageClear() override;
    void playWorldClear() override;

    void setVolume(float volume) override;
    float getVolume() const override;

    void stopAll() override;
};