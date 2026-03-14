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
    inline static float currentTickDelta = 0.0f;
    inline static std::unique_ptr<NetworkManagerState> state = nullptr;

    static void OnConnectedToServer();
    static void OnDisconnectedFromServer();
    static void OnMessageRecieved(const std::string& msg);
    static void OnError(const std::string& msg);

    #ifdef __EMSCRIPTEN__
    static EM_BOOL OnEMOpen(int type, const EmscriptenWebSocketOpenEvent* e, void* ud);
    static EM_BOOL OnEMClose(int type, const EmscriptenWebSocketCloseEvent* e, void* ud);
    static EM_BOOL OnEMMessage(int type, const EmscriptenWebSocketMessageEvent* e, void* ud);
    static EM_BOOL OnEMError(int type, const EmscriptenWebSocketErrorEvent* e, void* ud);
    #endif

    public:
    inline static uint32_t localClientId;
    inline static GameState networkGameState;
    inline static bool connected = false;

    static void Initialize();
    static void ConnectToServer();
    static void CreateLobby();
    static void JoinLobby(int lobbyCode);
    static void Shutdown();
};
