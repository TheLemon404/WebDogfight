#pragma once

#include <enet/enet.h>
#include <vector>

class NetworkManager {
    inline static ENetHost* client = nullptr;

    public:
    static void Initialize();
    static void ConnectToServer();
    static void CreateLobby();
    static void JoinLobby(int lobbyCode);
    static void Shutdown();
};
