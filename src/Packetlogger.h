#pragma once
#include <Windows.h>
#include "SafeQueue.h"

namespace Packetlogger
{
    void Initialize(SafeQueue* safeQueue);
    void SendPacket(LPCSTR szPacket);
    void HookRecv();
    void UnhookRecv();
}

