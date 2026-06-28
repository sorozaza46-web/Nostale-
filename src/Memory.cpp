#include "Memory.h"
#include <Psapi.h>

typedef BOOL(WINAPI* pGetModuleInformation)(HANDLE, HMODULE, LPMODULEINFO, DWORD);

DWORD Memory::FindPattern(char* pattern, char* mask)
{
    MODULEINFO mInfo = { 0 };
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule == 0) return NULL;

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
        mInfo.SizeOfImage = 0x1000000;
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
    return NULL; // Patladıysa çökertme, NULL dön ki menü anlasın
}

// Menüdeki butona basınca devreye girecek akıllı tarayıcı motoru
DWORD Memory::FindPatternByString(const char* szString)
{
    MODULEINFO mInfo = { 0 };
    HMODULE hModule = GetModuleHandle(NULL);
    if (!hModule) return NULL;
    
    GetModuleInformation(GetCurrentProcess(), hModule, &mInfo, sizeof(MODULEINFO));
    DWORD base = (DWORD)mInfo.lpBaseOfDll;
    DWORD size = mInfo.SizeOfImage;

    DWORD stringAddress = NULL;
    size_t strLen = strlen(szString);
    
    for (DWORD i = 0; i < size - strLen; i++) {
        if (memcmp((void*)(base + i), szString, strLen) == 0) {
            stringAddress = base + i;
            break;
        }
    }

    if (!stringAddress) return NULL;

    // Bellekte bu yazıyı push (0x68) eden yeri ara
    for (DWORD i = 0; i < size - 5; i++) {
        if (*(BYTE*)(base + i) == 0x68) { 
            DWORD pushTarget = *(DWORD*)(base + i + 1);
            if (pushTarget == stringAddress) {
                return base + i; 
            }
        }
    }
    return NULL;
}

bool Memory::Hook(void* toHook, void* ourFunc, int len)
{
    if (!toHook || len < 5) return false;
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
