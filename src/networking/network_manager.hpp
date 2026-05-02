#pragma once

#include "network_types.hpp"
#include "network_game.hpp"
#include <unordered_map>
#ifdef __EMSCRIPTEN__
#include <emscripten/websocket.h>
#else
#include "ixwebsocket/IXWebSocket.h"
#endif
#include <vector>

#define MAX_PLAYERS_PER_LOBBY 16

class NetworkManagerState {
    public:
    #ifdef __EMSCRIPTEN__
    EMSCRIPTEN_WEBSOCKET_T socket = 0;
    #else
    ix::WebSocket socket;
    #endif

    void SocketSendBinary(std::string buffer) {
        #ifdef __EMSCRIPTEN__
            emscripten_websocket_send_binary(socket, (void*)buffer.data(), buffer.size());
        #else
            socket.sendBinary(buffer);
        #endif
    }
    uint32_t lobbyId = -1;
};

class NetworkManager {
    float currentTickDelta = 0.0f;
    std::unique_ptr<NetworkManagerState> state = nullptr;

    void OnConnectedToServer();
    void OnDisconnectedFromServer();
    void OnMessageRecieved(const std::string& msg);
    void OnError(const std::string& msg);

    #ifdef __EMSCRIPTEN__
    static EM_BOOL OnEMOpen(int type, const EmscriptenWebSocketOpenEvent* e, void* ud);
    static EM_BOOL OnEMClose(int type, const EmscriptenWebSocketCloseEvent* e, void* ud);
    static EM_BOOL OnEMMessage(int type, const EmscriptenWebSocketMessageEvent* e, void* ud);
    static EM_BOOL OnEMError(int type, const EmscriptenWebSocketErrorEvent* e, void* ud);
    #endif

    float timeSinceLastStateSend = 0.0f;

    public:
    int GetLobbyId() const { return (int)state->lobbyId; }

    float interpolationFactor = 10.0f;

    std::mutex pendingStateChangeMutex;
    bool hasPendingStateChange = false;

    uint32_t localClientId;
    GameState lastNetworkGameState;
    GameState networkGameState;
    bool connected = false;

    void RequestStartFireGun(uint32_t targetNetworkID);
    void RequestStopFireGun();

    void Initialize();
    void Tick();
    void ConnectToServer();
    void CreateLobby();
    void JoinLobby(uint32_t lobbyCode);
    void Shutdown();
};
