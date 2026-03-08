#pragma once

#include <vector>

class NetworkManager {
    public:
    inline static bool connected = false;

    static void Initialize();
    static void ConnectToServer();
    static void CreateLobby();
    static void JoinLobby(int lobbyCode);
    static void Poll();
    static void Shutdown();
};
