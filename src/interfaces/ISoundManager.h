#pragma once

class ISoundManager
{
public:
    virtual ~ISoundManager() = default;

    // Gameplay sounds
    virtual void playBump() = 0;
    virtual void playBrick() = 0;
    virtual void playCoin() = 0;
    virtual void playDeath() = 0;
    virtual void playFireball() = 0;
    virtual void playFlagpole() = 0;
    virtual void playGameOver() = 0;
    virtual void playItem() = 0;
    virtual void playJump() = 0;
    virtual void playJumpSmall() = 0;
    virtual void playPowerUp() = 0;
    virtual void playStomp() = 0;
    virtual void playOneUp() = 0;

    // UI sounds
    virtual void playUIBump() = 0;
    virtual void playPause() = 0;
    virtual void playSaveGame() = 0;
    virtual void playSelectSuccess() = 0;
    virtual void playSelect() = 0;
    virtual void playStageClear() = 0;
    virtual void playWorldClear() = 0;

    // Global controls
    virtual void setVolume(float volume) = 0;
    virtual float getVolume() const = 0;
    virtual void stopAll() = 0;

    // Music
    virtual void playMenuMusic() = 0;
    virtual void stopMenuMusic() = 0;
    virtual void playGroundTheme() = 0;
    virtual void playCastleTheme() = 0;
    virtual void stopPlayMusic() = 0;
};