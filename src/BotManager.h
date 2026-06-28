#pragma once
#include <Windows.h>
#include <string>

enum class BotState {
    IDLE,
    CASTING_BUFFS,
    WAITING_FOR_FISH,
    LOOTING,
    NO_BAIT_STOPPED
};

struct Skill
{
    int ID;
    bool isReady;
    Skill(int skillID) : ID(skillID), isReady(true) {}
};

class BotManager
{
public:
    BotManager();
    int getPlayerID() const;
    int getSpLevel() const;
    void startFishing();
    void pickUpFish();
    void updateCooldown(int skillID, bool ready);
    bool isRunning() const;
    void start();
    void stop();
    void handleFishDetected(int type);
    void handleFishMissed();
    void handleNoBait();

    int pullDelayMs;
    BotState currentState;
    HWND hGuiWnd; 

private:
    void useSkill(Skill& skill);

    int* playerID;
    int8_t* spLevel;
    bool enabled;
    bool haveBait;

    Skill throwRodSkill{ 1 };
    Skill pickUpBait{ 2 };
    Skill baitSkill{ 3 };
    Skill expSkill{ 8 };
    Skill fishLineSkill{ 9 };
    Skill proBaitSkill{ 10 };
};

