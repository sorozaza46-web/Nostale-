#include "Memory.h"
#include <Psapi.h>

// Gizlilik için orijinal modül bilgilerini dinamik çözümleyen fonksiyon
typedef BOOL(WINAPI* pGetModuleInformation)(HANDLE, HMODULE, LPMODULEINFO, DWORD);

DWORD Memory::FindPattern(char* pattern, char* mask)
{
    MODULEINFO mInfo = { 0 };
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule == 0) return NULL;

    // K7: Dinamik import çağrısı ile anti-cheat analizini yanıltma
    HMODULE hPsapi = GetModuleHandleA("psapi.dll");
    if (!hPsapi) hPsapi = LoadLibraryA("psapi.dll");
    
    if (hPsapi) {
        pGetModuleInformation fnGetModuleInformation = (pGetModuleInformation)GetProcAddress(hPsapi, "GetModuleInformation");
        if (fnGetModuleInformation) {
            fnGetModuleInformation(GetCurrentProcess(), hModule, &mInfo, sizeof(MODULEINFO));
        }
    }

    if (mInfo.SizeOfImage == 0) {
        mInfo.lpBaseOfDll = (LPVOID)hModule;
        mInfo.SizeOfImage = 0x1000000; // Varsayılan tarama havuzu aralığı
    }

    DWORD base = (DWORD)mInfo.lpBaseOfDll;
    DWORD size = mInfo.SizeOfImage;
    DWORD patternLength = strlen(mask);

    for (DWORD i = 0; i < size - patternLength; i++)
    {
        bool found = true;
        for (DWORD j = 0; j < patternLength; j++)
            found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);

        if (found) return base + i;
    }
    return NULL;
}

bool Memory::Hook(void* toHook, void* ourFunc, int len)
{
    if (len < 5) return false;
    DWORD curProtection;
    VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);
    memset(toHook, 0x90, len);
    DWORD relativeAdress = ((DWORD)ourFunc - (DWORD)toHook) - 5;
    *(BYTE*)toHook = 0xE9;
    *(DWORD*)((DWORD)toHook + 1) = relativeAdress;
    DWORD temp;
    VirtualProtect(toHook, len, curProtection, &temp);
    return true;
}

void Memory::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
    DWORD oldprotect;
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldprotect, &oldprotect);
}

