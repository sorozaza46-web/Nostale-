#pragma once
#include <Windows.h> // LPCSTR ve Win32 veri tipleri için ŞART
#include "SafeQueue.h"

namespace Packetlogger
{
    void Initialize(SafeQueue* safeQueue);
    bool AutoFindAddresses(); 
    void SendPacket(LPCSTR szPacket);
    void HookRecv();
    void UnhookRecv();
}
