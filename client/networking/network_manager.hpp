#pragma once

#include <vector>

struct NetworkEntity {
    unsigned int id;
};

struct Lobby {
    std::vector<NetworkEntity> networkEntities;
};

class NetworkManager {
    public:
    static void CreateLobby();
    static void JoinLobby(int lobbyCode);
};
