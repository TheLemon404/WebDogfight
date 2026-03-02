#pragma once

#include <enet/enet.h>
#include <vector>

class NetworkManager {
    inline static ENetAddress address;
    inline static ENetHost* client = nullptr;
    inline static ENetPeer* peer = nullptr;
    inline static ENetEvent event;

    public:
    inline static bool connected = false;

    static void Initialize();
    static void ConnectToServer();
    static void CreateLobby();
    static void JoinLobby(int lobbyCode);
    static void Poll();
    static void Shutdown();
};
