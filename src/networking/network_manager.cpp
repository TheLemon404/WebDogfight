#include "network_manager.hpp"
#include "enet/enet.h"

#include <iostream>

#define SERVER_PORT 1234
#define CONNECTION_WAIT_TIME 1000

void NetworkManager::Initialize() {
#ifndef __EMSCRIPTEN__
    if(enet_initialize() != 0){
        throw std::runtime_error("Failed to initialize ENet");
    }

    atexit(enet_deinitialize);

    address.host = ENET_HOST_ANY;
    address.port = SERVER_PORT;

    std::cout << "Succesfully initialized ENet" << std::endl;
#endif
}

void NetworkManager::ConnectToServer() {
#ifndef __EMSCRIPTEN__
    client = enet_host_create(NULL, 1, 2, 0, 0);

    if(client == nullptr) {
        throw std::runtime_error("failed to create enet client");
    }

    enet_address_set_host(&address, "127.0.0.1");
    peer = enet_host_connect(client, &address, 2, 0);
    if(peer == nullptr) {
        throw std::runtime_error("No available peers for initializing an enet connection");
    }

    if(enet_host_service(client, &event, CONNECTION_WAIT_TIME) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "connected to server" << std::endl;
        connected = true;
    }
    else {
        enet_peer_reset(peer);
        std::cout << "failed to connect to server" << std::endl;
    }
#endif
}

void NetworkManager::CreateLobby() {

}

void NetworkManager::JoinLobby(int lobbyCode) {

}

void NetworkManager::Poll() {
#ifndef __EMSCRIPTEN__
    ENetEvent event;
        while(enet_host_service(client, &event, 0) > 0) {
            switch(event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout << "connect event received" << std::endl;
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    connected = false;
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    enet_packet_destroy(event.packet);
                    break;
            }
        }
#endif
}

void NetworkManager::Shutdown() {
#ifndef __EMSCRIPTEN__
    if(peer && connected) {
            enet_peer_disconnect(peer, 0);
            ENetEvent event;
            while(enet_host_service(client, &event, 3000) > 0) {
                if(event.type == ENET_EVENT_TYPE_DISCONNECT) break;
                if(event.type == ENET_EVENT_TYPE_RECEIVE) enet_packet_destroy(event.packet);
            }
        }
    enet_host_destroy(client);
#endif
}
