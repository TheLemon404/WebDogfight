#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten/websocket.h>
#else
#include "ixwebsocket/IXWebSocket.h"
#endif
#include <vector>

struct NetworkManagerState {
    #ifdef __EMSCRIPTEN__
    EMSCRIPTEN_WEBSOCKET_T socket = 0;
    #else
    ix::WebSocket socket;
    #endif
};

class NetworkManager {
    inline static std::unique_ptr<NetworkManagerState> state = nullptr;

    static void OnConnectedToServer();
    static void OnMessageRecieved(const std::string& msg);

    #ifdef __EMSCRIPTEN__
    static EM_BOOL OnEMOpen(int type, const EmscriptenWebSocketOpenEvent* e, void* ud);
    static EM_BOOL OnEMClose(int type, const EmscriptenWebSocketCloseEvent* e, void* ud);
    static EM_BOOL OnEMMessage(int type, const EmscriptenWebSocketMessageEvent* e, void* ud);
    static EM_BOOL OnEMError(int type, const EmscriptenWebSocketErrorEvent* e, void* ud);
    #endif

    public:
    inline static bool connected = false;

    static void Initialize();
    static void ConnectToServer();
    static void CreateLobby();
    static void JoinLobby(int lobbyCode);
    static void Shutdown();
};
