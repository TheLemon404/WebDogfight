#pragma once

#include "network_types.hpp"
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class ClientState {
    public:
    bool inGame = false;
    bool shotDown = false;

    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 velocity;

    std::string name = "guest";

    std::string Serialize() {
        return Packet()
            .WriteU8(inGame)
            .WriteU8(shotDown)
            .WriteF32(position.x)
            .WriteF32(position.y)
            .WriteF32(position.z)
            .WriteF32(rotation.x)
            .WriteF32(rotation.y)
            .WriteF32(rotation.z)
            .WriteF32(rotation.w)
            .WriteF32(velocity.x)
            .WriteF32(velocity.y)
            .WriteF32(velocity.z)
            .WriteU8((uint8_t)name.length())
            .WriteBuffer(name)
            .Build();
    }

    //-- IMPORTANT -- make sure to NOT rewind packet before passed into function
    void Deserialize(Packet& packet) {
        inGame = packet.ReadU8();
        shotDown = packet.ReadU8();
        position.x = packet.ReadF32();
        position.y = packet.ReadF32();
        position.z = packet.ReadF32();
        rotation.x = packet.ReadF32();
        rotation.y = packet.ReadF32();
        rotation.z = packet.ReadF32();
        rotation.w = packet.ReadF32();
        velocity.x = packet.ReadF32();
        velocity.y = packet.ReadF32();
        velocity.z = packet.ReadF32();
        size_t nameSize = packet.ReadU8();
        name = packet.ReadBuffer(nameSize);
    }
};

struct GameState {
    std::unordered_map<uint32_t, ClientState> clientStates;
    float lastUpdateTimeStamp = 0.0f;

    std::string Serialize() {
        Packet packet = Packet();
        packet.WriteU8(clientStates.size());
        for(auto& clientState : clientStates) {
            packet.WriteU32(clientState.first).WriteBuffer(clientState.second.Serialize());
        }
        return packet.Build();
    }

    void Deserialize(Packet& packet) {
        clientStates.clear();
        //the u8 at the beginning of the packet is the number of clients
        uint8_t numClients = packet.ReadU8();
        for(int i = 0; i < numClients; i++) {
            uint32_t clientId = packet.ReadU32();
            clientStates[clientId].Deserialize(packet);
        }
    }
};

class NetworkClient {
    public:
    uint32_t lobbyId = -1;
    std::string name = "guest";

    NetworkClient() {};
};

class Lobby {
    public:
    GameState gameState;
};
