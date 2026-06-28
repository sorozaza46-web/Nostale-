#pragma once
#include <string>
#include "BotManager.h"

namespace PacketManager
{
    void Initialize(BotManager* botMan);
    void HandlePacket(std::string packet);
}

