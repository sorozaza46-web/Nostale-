#include "WinGui.h"
#include "Packetlogger.h"
#include <commctrl.h> // HATA ÇÖZÜMÜ: Slider/Trackbar makroları için şart
#include <string>

// Linker'a Comctl32 kütüphanesini bağlamasını söylüyoruz
#pragma comment(lib, "comctl32.lib")

HWND hMainWnd = NULL;
HWND hBtnStart = NULL;
HWND hBtnStop = NULL;
HWND hBtnAutoFix = NULL; 
HWND hStatusLabel = NULL;
HWND hDelaySlider = NULL;
HWND hDelayValLabel = NULL;

BotManager* pBotManager = nullptr;
SafeQueue* pQueue = nullptr;

extern DWORD RecvHookAddy; 

void UpdateUiElements()
{
    if (!pBotManager) return;

    if (RecvHookAddy == NULL) {
        SetWindowTextA(hStatusLabel, "Durum: Adresler Hatali! (Patch)");
        EnableWindow(hBtnStart, FALSE); 
        EnableWindow(hBtnStop, FALSE);
        ShowWindow(hBtnAutoFix, SW_SHOW); 
        return;
    }

    ShowWindow(hBtnAutoFix, SW_HIDE); 
    std::string statusText = "Durum: ";
    switch (pBotManager->currentState) {
        case BotState::IDLE: statusText += "Pasif"; break;
        case BotState::CASTING_BUFFS: statusText += "Hazirlaniyor..."; break;
        case BotState::WAITING_FOR_FISH: statusText += "Bekleniyor..."; break;
        case BotState::LOOTING: statusText += "YAKALANDI!"; break;
        case BotState::NO_BAIT_STOPPED: statusText += "YEM BITTI!"; break;
    }
    SetWindowTextA(hStatusLabel, statusText.c_str());
    EnableWindow(hBtnStart, !pBotManager->isRunning());
    EnableWindow(hBtnStop, pBotManager->isRunning());
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        // Common Controls bileşenlerini (Slider) belleğe yükle
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_BAR_CLASSES;
        InitCommonControlsEx(&icex);

        CreateWindowA("STATIC", "Nova Suite Internal Framework", WS_VISIBLE | WS_CHILD | SS_CENTER, 10, 10, 260, 20, hwnd, NULL, NULL, NULL);
        hStatusLabel = CreateWindowA("STATIC", "Durum: Kontrol ediliyor...", WS_VISIBLE | WS_CHILD, 20, 40, 240, 20, hwnd, NULL, NULL, NULL);
        
        hBtnStart = CreateWindowA("BUTTON", "Baslat (F5)", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 20, 70, 110, 30, hwnd, (HMENU)101, NULL, NULL);
        hBtnStop = CreateWindowA("BUTTON", "Durdur (F8)", WS_VISIBLE | WS_CHILD, 140, 70, 110, 30, hwnd, (HMENU)102, NULL, NULL);
        hBtnAutoFix = CreateWindowA("BUTTON", "Otomatik Adres Bul (Auto-Fix)", WS_CHILD | BS_PUSHBUTTON, 20, 110, 230, 30, hwnd, (HMENU)200, NULL, NULL);

        CreateWindowA("STATIC", "Gecikme:", WS_VISIBLE | WS_CHILD, 20, 150, 80, 20, hwnd, NULL, NULL, NULL);
        hDelayValLabel = CreateWindowA("STATIC", "1000", WS_VISIBLE | WS_CHILD, 110, 150, 50, 20, hwnd, NULL, NULL, NULL);
        
        hDelaySlider = CreateWindowA(TRACKBAR_CLASSA, "", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_HORZ, 20, 170, 230, 30, hwnd, (HMENU)103, NULL, NULL);
        SendMessage(hDelaySlider, TBM_SETRANGE, TRUE, MAKELONG(500, 3000));
        SendMessage(hDelaySlider, TBM_SETPOS, TRUE, 1000);
        break;

    case WM_COMMAND:
        if (LOWORD(wp) == 101) { pBotManager->start(); pBotManager->startFishing(); }
        if (LOWORD(wp) == 102) { pBotManager->stop(); }
        
        if (LOWORD(wp) == 200) { 
            if (Packetlogger::AutoFindAddresses()) {
                Packetlogger::HookRecv(); 
                MessageBoxA(hwnd, "Yeni hafiza adresleri basariyla bulundu!", "Nova Suite", MB_OK | MB_ICONINFORMATION);
            } else {
                MessageBoxA(hwnd, "Otomatik tarama basarisiz.", "Nova Suite Error", MB_OK | MB_ICONERROR);
            }
        }
        UpdateUiElements();
        break;

    case WM_HSCROLL:
        if ((HWND)lp == hDelaySlider) {
            int pos = SendMessage(hDelaySlider, TBM_GETPOS, 0, 0);
            SetWindowTextA(hDelayValLabel, std::to_string(pos).c_str());
            if (pBotManager) pBotManager->pullDelayMs = pos;
        }
        break;

    case WM_USER + 100: UpdateUiElements(); break;
    case WM_CLOSE: ShowWindow(hwnd, SW_HIDE); break;
    default: return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

void WinGui::RegisterWindowClass(HINSTANCE hInst)
{
    WNDCLASSEXA wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "NV_ST_WIN_CLASS_MNG_X86";
    RegisterClassExA(&wc);
}

void WinGui::CreateGuiWindow(HINSTANCE hInst, BotManager* botMan, SafeQueue* queue)
{
    pBotManager = botMan;
    pQueue = queue;

    // HATA ÇÖZÜMÜ: En sondaki eksik 12. argüman (NULL) eklendi!
    hMainWnd = CreateWindowExA(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, "NV_ST_WIN_CLASS_MNG_X86", "Nova Suite Panel", 
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, 
        120, 120, 300, 260, NULL, NULL, hInst, NULL);
        
    pBotManager->hGuiWnd = hMainWnd;
    UpdateUiElements();
}

HWND WinGui::GetWindowHandle() { return hMainWnd; }
