#include "network_manager.hpp"

#include <iostream>


void NetworkManager::Initialize() {
    client = enet_host_create(NULL, 1, 2, 0, 0);

    if(client == nullptr) {
        throw std::runtime_error("failed to create enet client");
    }
}

void NetworkManager::ConnectToServer() {

}

void NetworkManager::CreateLobby() {

}

void NetworkManager::JoinLobby(int lobbyCode) {

}

void NetworkManager::Shutdown() {
    enet_host_destroy(client);
}
