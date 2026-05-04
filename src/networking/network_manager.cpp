#include "network_manager.hpp"

#include "../io/time.hpp"
#include "../gameplay/scene_manager.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_int3_sized.hpp"
#include "network_game.hpp"
#include "network_types.hpp"
#include <memory>
#include <sys/stat.h>

#ifndef __EMSCRIPTEN__
#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocketMessageType.h"
#endif

#include <iostream>
#include "../application.hpp"

#define SERVER_URL "wss://webdogfightserver.onrender.com"
//#define SERVER_URL "ws://127.0.0.1:1234/"
#define HEARTBEAT_PING_INTERVAL 45
#define STATE_SEND_INTERVAL 0.05

#ifdef __EMSCRIPTEN__
EM_BOOL NetworkManager::OnEMOpen(int type, const EmscriptenWebSocketOpenEvent* e, void* ud) {
    NetworkManager* networkManager = static_cast<NetworkManager*>(ud);

    networkManager->OnConnectedToServer();
    return EM_TRUE;
}

EM_BOOL NetworkManager::OnEMClose(int type, const EmscriptenWebSocketCloseEvent* e, void* ud) {
    NetworkManager* networkManager = static_cast<NetworkManager*>(ud);

    networkManager->OnDisconnectedFromServer();
    return EM_TRUE;
}

EM_BOOL NetworkManager::OnEMMessage(int type, const EmscriptenWebSocketMessageEvent* e, void* ud) {
    NetworkManager* networkManager = static_cast<NetworkManager*>(ud);

    if(e->isText) return EM_TRUE;
    std::string data(e->data, e->data + e->numBytes);
    networkManager->OnMessageRecieved(data);
    return EM_TRUE;
}

EM_BOOL NetworkManager::OnEMError(int type, const EmscriptenWebSocketErrorEvent* e, void* ud) {
    NetworkManager* networkManager = static_cast<NetworkManager*>(ud);

    networkManager->OnError("websocket encountered an error (emscripten does not provide more event details)");
    return EM_TRUE;
}
#endif

void NetworkManager::OnConnectedToServer() {
    std::cout << "connected to server" << std::endl;
    connected = true;
    //state->SocketSendBinary(Packet().WritePacketType(PacketType::NAME_UPDATE))
    state->SocketSendBinary(Packet().WritePacketType(PacketType::JOIN_RANDOM_LOBBY).Build());
}

void NetworkManager::OnDisconnectedFromServer() {
    std::cout << "disconnected from server" << std::endl;

    //TODO: figure out why this keeps being called!!!
}

void NetworkManager::OnMessageRecieved(const std::string& msg) {
    std::unique_ptr<Application>& app = Application::GetInstance();

    Packet packet = Packet(msg);
    PacketType packetType = packet.ReadPacketType();
    switch(packetType) {
        case PacketType::CONNECTED_TO_SERVER:
        {
            localClientId = packet.ReadU32();
            break;
        }
        case PacketType::LOBBY_JOINED:
        {
            std::lock_guard<std::mutex> lock(pendingStateChangeMutex);
            state->lobbyId = packet.ReadU32();
            if (state->lobbyId == 0) {
                std::cout << "failed to join lobby" << std::endl;
                break;
            }

            std::cout << "lobby joined: " << state->lobbyId << std::endl;
            ClientState preservedClientState = networkGameState.clientStates[localClientId];
            networkGameState.Deserialize(packet);
            networkGameState.clientStates[localClientId] = preservedClientState;
            app->sceneManager.currentScene->SpawnAndDespawnNetworkEntities(lastNetworkGameState, networkGameState);
            hasPendingStateChange = true;
            break;
        }
        case PacketType::LOBBY_LEFT:
        {
            std::lock_guard<std::mutex> lock(pendingStateChangeMutex);
            state->lobbyId = 0;
            networkGameState.clientStates.clear();
            hasPendingStateChange = true;
            break;
        }
        case PacketType::LOBBY_STATE_UPDATED:
        {
            std::lock_guard<std::mutex> lock(pendingStateChangeMutex);
            lastNetworkGameState = networkGameState;

            ClientState preservedClientState = networkGameState.clientStates[localClientId];
            networkGameState.Deserialize(packet);
            networkGameState.lastUpdateTimeStamp = app->clock.currentTime;
            //this nasty code ensures we cannot overwrite our being shot down by the server
            bool externalShotDownCommand = networkGameState.clientStates[localClientId].shotDown;
            bool externalExplodedCommand = networkGameState.clientStates[localClientId].exploded;
            networkGameState.clientStates[localClientId] = preservedClientState;
            networkGameState.clientStates[localClientId].shotDown = externalShotDownCommand;
            networkGameState.clientStates[localClientId].exploded = externalExplodedCommand;
            app->sceneManager.currentScene->SpawnAndDespawnNetworkEntities(lastNetworkGameState, networkGameState);
            hasPendingStateChange = true;
            break;
        }
        case PacketType::SHOOT_DOWN_DEMAND:
        {
            if(onShotDownDemand != nullptr){
                onShotDownDemand();
            }
            break;
        }
        case PacketType::EXPLODE_DEMAND:
        {
            if(onExplodeDemand != nullptr) {
                onExplodeDemand();
            }
            break;
        }
    }
}

void NetworkManager::OnError(const std::string& msg) {
    std::cout << "Network Error: " << msg << std::endl;
}

void NetworkManager::RequestFireGun(uint32_t targetNetworkID) {
    state->SocketSendBinary(
        Packet()
        .WritePacketType(PacketType::REQUEST_FIRE_GUN)
        .WriteU32(targetNetworkID)
        .Build()
    );
}

void NetworkManager::Initialize() {
#ifdef __EMSCRIPTEN__
    std::cout << "Initializing emscripten networking backend..." << std::endl;
#else
    ix::initNetSystem();
#endif

    std::cout << "Network backend initialized successfully" << std::endl;
}

void NetworkManager::Tick() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    timeSinceLastStateSend += app->clock.deltaTime;

    if(timeSinceLastStateSend >= STATE_SEND_INTERVAL && connected) {
        ClientState localClientState = networkGameState.clientStates[localClientId];
        state->SocketSendBinary(
            Packet()
            .WritePacketType(PacketType::UPDATE_CLIENT_STATE)
            .WriteBuffer(localClientState.Serialize())
            .Build()
        );
        timeSinceLastStateSend = 0.0f;
    }
}

void NetworkManager::ConnectToServer() {
    state = std::make_unique<NetworkManagerState>();
    std::cout << "connected to server" << std::endl;

#ifdef __EMSCRIPTEN__
    EmscriptenWebSocketCreateAttributes attrs;
    emscripten_websocket_init_create_attributes(&attrs);
    attrs.url = SERVER_URL;
    attrs.protocols = nullptr;
    attrs.createOnMainThread = true;
    state->socket = emscripten_websocket_new(&attrs);

    emscripten_websocket_set_onopen_callback(state->socket, this, NetworkManager::OnEMOpen);
    emscripten_websocket_set_onclose_callback(state->socket, this, NetworkManager::OnEMClose);
    emscripten_websocket_set_onmessage_callback(state->socket, this, NetworkManager::OnEMMessage);
    emscripten_websocket_set_onerror_callback(state->socket, this, NetworkManager::OnEMError);
#else
    state->socket.setUrl(SERVER_URL);
    state->socket.disablePerMessageDeflate();
    state->socket.disableAutomaticReconnection();
    state->socket.setPingInterval(HEARTBEAT_PING_INTERVAL);
    state->socket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
        switch(msg->type) {
            case ix::WebSocketMessageType::Open:
                OnConnectedToServer();
                break;
            case ix::WebSocketMessageType::Close:
                OnDisconnectedFromServer();
                break;
            case ix::WebSocketMessageType::Message:
                OnMessageRecieved(msg->str);
                break;
            case ix::WebSocketMessageType::Error:
                OnError(msg->errorInfo.reason);
                break;
        }
    });

    std::cout << "websocket created" << std::endl;

    state->socket.start();
#endif
}

void NetworkManager::CreateLobby() {
    state->SocketSendBinary(
        Packet()
        .WritePacketType(PacketType::CREATE_LOBBY)
        .Build()
    );
}

void NetworkManager::JoinLobby(uint32_t lobbyCode) {
    state->SocketSendBinary(
        Packet()
        .WritePacketType(PacketType::JOIN_LOBBY)
        .WriteU32(lobbyCode)
        .Build()
    );
}

void NetworkManager::Shutdown() {
#ifdef __EMSCRIPTEN__
    emscripten_websocket_delete(state->socket);
#else
    if(state && connected) {
        state->socket.stop();
    }
    ix::uninitNetSystem();
#endif
    std::cout << "Network backend shutdown successfully" << std::endl;
}
