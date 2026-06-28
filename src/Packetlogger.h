// Packetlogger.h içerisine eklenecek fonksiyon tanımı:
namespace Packetlogger {
    void Initialize(SafeQueue* safeQueue);
    bool AutoFindAddresses(); // Yeni: Menüden çağrılacak otomatik bulucu
    void SendPacket(LPCSTR szPacket);
    void HookRecv();
    void UnhookRecv();
}
