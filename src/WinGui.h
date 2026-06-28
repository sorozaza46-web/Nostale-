#pragma once
#include <Windows.h>
#include "BotManager.h"
#include "SafeQueue.h"

namespace WinGui
{
    void CreateGuiWindow(HINSTANCE hInst, BotManager* botMan, SafeQueue* queue);
    void RegisterWindowClass(HINSTANCE hInst);
    HWND GetWindowHandle();
}

