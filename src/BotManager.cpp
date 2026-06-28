#include "BotManager.h"
#include "Memory.h"
#include "Packetlogger.h"

BotManager::BotManager()
{
    pullDelayMs = 1000;
    currentState = BotState::IDLE;
    enabled = false;
    haveBait = true;
    hGuiWnd = NULL;

    // Sizin orijinal bayt imza verileriniz (Birebir korundu)
    DWORD playerPtr = Memory::FindPattern((char*)"\xA1\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xA1\x00\x00\x00\x00\x83\x38\x00\x76", (char*)"x????????xx????xx?x");
    if (playerPtr) {
        playerID = (int*)(**(DWORD**)(playerPtr + 1) + 0x24);
    } else {
        playerID = new int(0);
    }

    DWORD spPtr = Memory::FindPattern((char*)"\xA1\x00\x00\x00\x00\x8B\x00\x33\xD2\xE8", (char*)"x????xxxxx");
    if (spPtr) {
        spLevel = (int8_t*)(***(DWORD***)(spPtr + 1) + 0x04);
    } else {
        spLevel = new int8_t(0);
    }
}

int BotManager::getPlayerID() const { return playerID ? *playerID : 0; }
int BotManager::getSpLevel() const { return spLevel ? *spLevel : 0; }
bool BotManager::isRunning() const { return enabled; }

void BotManager::start() {
    if (enabled) return;
    enabled = true;
    haveBait = true;
    currentState = BotState::CASTING_BUFFS;
    if (hGuiWnd) PostMessage(hGuiWnd, WM_USER + 100, 0, 0);
}

void BotManager::stop() {
    enabled = false;
    currentState = BotState::IDLE;
    if (hGuiWnd) PostMessage(hGuiWnd, WM_USER + 100, 0, 0);
}

void BotManager::useSkill(Skill& skill) {
    if (!enabled) return;
    std::string packet = "u_s " + std::to_string(skill.ID) + " 1 " + std::to_string(getPlayerID());
    Packetlogger::SendPacket(packet.c_str());
}

void BotManager::startFishing()
{
    if (!enabled) return;
    currentState = BotState::CASTING_BUFFS;
    if (hGuiWnd) PostMessage(hGuiWnd, WM_USER + 100, 0, 0);

    int sp = getSpLevel();
    if (sp >= 25 && fishLineSkill.isReady) { useSkill(fishLineSkill); Sleep(200); }
    if (sp >= 25 && expSkill.isReady) { useSkill(expSkill); Sleep(200); }
    if (sp >= 45 && proBaitSkill.isReady) { useSkill(proBaitSkill); currentState = BotState::WAITING_FOR_FISH; if (hGuiWnd) PostMessage(hGuiWnd, WM_USER + 100, 0, 0); return; }
    if (sp >= 3 && baitSkill.isReady) { useSkill(baitSkill); Sleep(200); }

    useSkill(throwRodSkill);
    currentState = BotState::WAITING_FOR_FISH;
    if (hGuiWnd) PostMessage(hGuiWnd, WM_USER + 100, 0, 0);
}

void BotManager::pickUpFish()
{
    if (!enabled) return;
    useSkill(pickUpBait);
}

void BotManager::handleFishDetected(int type)
{
    if (!enabled) return;
    currentState = BotState::LOOTING;
    if (hGuiWnd) PostMessage(hGuiWnd, WM_USER + 100, 0, 0);
    Sleep(pullDelayMs);
    pickUpFish();
}

void BotManager::handleFishMissed()
{
    if (!enabled) return;
    Sleep(1000);
    startFishing();
}

void BotManager::handleNoBait()
{
    haveBait = false;
    stop();
    currentState = BotState::NO_BAIT_STOPPED;
    if (hGuiWnd) PostMessage(hGuiWnd, WM_USER + 100, 0, 0);
}

void BotManager::updateCooldown(int skillID, bool ready)
{
    if (skillID == 3) baitSkill.isReady = ready;
    if (skillID == 8) expSkill.isReady = ready;
    if (skillID == 9) fishLineSkill.isReady = ready;
    if (skillID == 10) proBaitSkill.isReady = ready;
}

