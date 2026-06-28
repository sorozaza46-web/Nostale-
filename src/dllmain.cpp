#include <Windows.h>
#include "Packetlogger.h"
#include "PacketManager.h"
#include "BotManager.h"
#include "SafeQueue.h"
#include "WinGui.h"

DWORD WINAPI MainThread(LPVOID param)
{
    HINSTANCE hInst = (HINSTANCE)param;
    BotManager botManager;
    SafeQueue qRecv;

    PacketManager::Initialize(&botManager);
    Packetlogger::Initialize(&qRecv);
    Packetlogger::HookRecv();

    WinGui::RegisterWindowClass(hInst);
    WinGui::CreateGuiWindow(hInst, &botManager, &qRecv);

    MSG msg;
    while (true)
    {
        // 1. GUI Event Döngüsü
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // 2. Ağ Paket İşleme Döngüsü
        while (!qRecv.empty()) {
            char* packet = qRecv.front();
            if (packet) {
                PacketManager::HandlePacket(std::string(packet));
            }
            qRecv.pop();
        }

        // Hotkey Kontrolleri
        if (GetAsyncKeyState(VK_F5) & 1) {
            if (!botManager.isRunning()) {
                botManager.start();
                botManager.startFishing();
            }
        }
        if (GetAsyncKeyState(VK_F8) & 1) {
            if (botManager.isRunning()) {
                botManager.stop();
            }
        }
        if (GetAsyncKeyState(VK_F12) & 1) {
            break; // Döngüden çık, hileyi bellekten güvenle kaldır
        }

        Sleep(10);
    }

    Packetlogger::UnhookRecv();
    if (WinGui::GetWindowHandle()) {
        DestroyWindow(WinGui::GetWindowHandle());
    }
    
    FreeLibraryAndExitThread(hInst, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        // Gizlilik için Thread parametresi ile güvenli çalıştırma
        CreateThread(NULL, 0, MainThread, hModule, 0, NULL);
    }
    return TRUE;
}

