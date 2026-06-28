#include "Packetlogger.h"
#include "NostaleString.h"
#include "Memory.h"

SafeQueue* qRecv;
DWORD SendAddy = 0;
DWORD RecvHookAddy = 0;
DWORD TNTClient = 0;
DWORD originalCallAddy = 0;
DWORD jmpBackAddy = 0;
char* packetPtr;

void __declspec(naked) CustomRecv()
{
    __asm {
        pushad
        pushfd
        mov packetPtr, edx
    }
    qRecv->push(packetPtr);
    __asm {
        popfd
        popad
        call originalCallAddy
        jmp jmpBackAddy
    }
}

void Packetlogger::Initialize(SafeQueue* safeQueue)
{
    qRecv = safeQueue;
    // Orijinal paket imza baytlarınız tam olarak korundu
    RecvHookAddy = Memory::FindPattern((char*)"\xe8\x00\x00\x00\x00\x33\xc0\x55\x68\x00\x00\x00\x00\x64\xff\x00\x64\x89\x00\x8d\\x45\x00\x8b\x55", (char*)"x????xxxx????xx?xx?xx?xx");
    TNTClient = Memory::FindPattern((char*)"\xA1\x00\x00\x00\x00\x8B\x00\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x8B\\x00\x33\xD2\x89\x10", (char*)"x????xxx????x????xxxxxx") + 1;
    SendAddy = Memory::FindPattern((char*)"\x55\x8B\xEC\x6A\x00\x6A\x00\x53\x8B\xD8", (char*)"xxxxxxxxxx");
}

void Packetlogger::SendPacket(LPCSTR szPacket)
{
    if (!SendAddy || !TNTClient) return;
    NostaleStringA str(szPacket);
    char* packet = str.get();
    __asm {
        mov eax, dword ptr ds:[TNTClient]
        mov eax, dword ptr ds:[eax]
        mov edx, packet
        call SendAddy
    }
}

void Packetlogger::HookRecv()
{
    if (!RecvHookAddy) return;
    originalCallAddy = *(DWORD*)(RecvHookAddy + 1) + RecvHookAddy + 5;
    jmpBackAddy = RecvHookAddy + 5;
    Memory::Hook((void*)RecvHookAddy, CustomRecv, 5);
}

void Packetlogger::UnhookRecv() { }

