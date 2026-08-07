#pragma once
class PlayerManager;

// Command Pattern
class Command {
public:
    virtual ~Command() = default;
    virtual void execute(PlayerManager& player) = 0;
};

class JumpCommand : public Command {
public:
    void execute(PlayerManager& player) override;
};

class StopJumpCommand : public Command {
public:
    void execute(PlayerManager& player) override;
};

class MoveLeftCommand : public Command {
public:
    void execute(PlayerManager& player) override;
};

class MoveRightCommand : public Command {
public:
    void execute(PlayerManager& player) override;
};

class StopHorizontalCommand : public Command {
public:
    void execute(PlayerManager& player) override;
}; 