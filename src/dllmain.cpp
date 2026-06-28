#include <Windows.h>
#include "Packetlogger.h"
#include "BotManager.h"
#include "PacketManager.h"
#include "WinGui.h"

BotManager botManager;
SafeQueue qRecv;

DWORD WINAPI MainThread(LPVOID param)
{
    // Arayüz Sınıfı Kaydı
    WinGui::RegisterWindowClass((HINSTANCE)param);

    // Motorları Başlat
    PacketManager::Initialize(&botManager);
    Packetlogger::Initialize(&qRecv);
    Packetlogger::HookRecv();

    // Görsel Arayüz Penceresini Aç
    WinGui::CreateGuiWindow((HINSTANCE)param, &botManager, &qRecv);

    // Ana Döngü (F12 tuşuna basılana kadar hileyi canlı tutar)
    while (!GetAsyncKeyState(VK_F12))
    {
        // Gelen kuyruktaki paketleri işle
        while (!qRecv.empty())
        {
            std::string packet = qRecv.front();
            PacketManager::HandlePacket(packet);
            qRecv.pop();
        }

        // Arka planda pencerelerin durumunu tetikle (Mesaj döngüsü pompalama)
        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Menüyü asenkron güncellemek için sinyal gönder
        SendMessage(WinGui::GetWindowHandle(), WM_USER + 100, 0, 0);

        Sleep(10);
    }

    // Kapatma/Temizleme Aşaması (Unhook)
    Packetlogger::UnhookRecv();
    DestroyWindow(WinGui::GetWindowHandle());
    
    Sleep(100);
    FreeLibraryAndExitThread((HMODULE)param, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, MainThread, hModule, 0, 0);
    }
    return TRUE;
}
