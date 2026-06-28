#include "PacketManager.h"
#include <sstream>
#include <vector>

BotManager* botManager;

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void PacketManager::Initialize(BotManager* botMan) { botManager = botMan; }

void PacketManager::HandlePacket(std::string packet)
{
    if (packet.empty()) return;
    if (packet.back() == '\n' || packet.back() == '\r') packet.pop_back();

    std::vector<std::string> tokens = split(packet, ' ');
    if (tokens.empty()) return;

    std::string opcode = tokens[0];

    if (opcode == "guri") {
        if (tokens.size() < 5) return;
        std::string type = tokens[4];
        if (type == "30" || type == "31") {
            botManager->handleFishDetected(std::stoi(type));
        } else if (type == "0") {
            botManager->handleFishMissed();
        }
    }
    else if (opcode == "sayi") {
        if (tokens.size() < 5) return;
        if (tokens[4] == "2497") { 
            botManager->handleNoBait();
        }
    }
    else if (opcode == "sr") {
        if (tokens.size() < 3) return;
        int skillId = std::stoi(tokens[1]);
        int ready = std::stoi(tokens[2]);
        botManager->updateCooldown(skillId, ready == 1);
    }
    else if (opcode == "c_map" || opcode == "tp") {
        botManager->stop();
    }
}

