#pragma once
#include <Windows.h>

namespace Memory
{
    DWORD FindPattern(char* pattern, char* mask);
    DWORD FindPatternByString(const char* szString); // Yeni: Akıllı dinamik arayıcı
    bool Hook(void* toHook, void* ourFunc, int len);
    void Patch(BYTE* dst, BYTE* src, unsigned int size);
}
