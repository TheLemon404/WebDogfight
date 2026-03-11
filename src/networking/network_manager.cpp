#include "network_manager.hpp"

#include "../gameplay/scene_manager.hpp"
#include "../gameplay/menu_scene.hpp"
#include "network_types.hpp"
#include <memory>

#ifndef __EMSCRIPTEN__
#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocketMessageType.h"
#endif

#include <iostream>

#define SERVER_URL "ws://127.0.0.1:1234/"
#define HEARTBEAT_PING_INTERVAL 45

#ifdef __EMSCRIPTEN__
EM_BOOL NetworkManager::OnEMOpen(int type, const EmscriptenWebSocketOpenEvent* e, void* ud) {
    OnConnectedToServer();
    return EM_TRUE;
}

EM_BOOL NetworkManager::OnEMClose(int type, const EmscriptenWebSocketCloseEvent* e, void* ud) {
    OnDisconnectedFromServer();
    return EM_TRUE;
}

EM_BOOL NetworkManager::OnEMMessage(int type, const EmscriptenWebSocketMessageEvent* e, void* ud) {
    if(e->isText) return EM_TRUE;
    std::string data(e->data, e->data + e->numBytes);
    OnMessageRecieved(data);
    return EM_TRUE;
}

EM_BOOL NetworkManager::OnEMError(int type, const EmscriptenWebSocketErrorEvent* e, void* ud) {
    OnError("websocket encountered an error (emscripten does not provide more event details)");
    return EM_TRUE;
}
#endif

void NetworkManager::OnConnectedToServer() {
    std::cout << "connected to server" << std::endl;
    connected = true;

    const std::string message = Packet().WritePacketType(PacketType::JOIN_RANDOM_LOBBY).Build();

    #ifdef __EMSCRIPTEN__
        emscripten_websocket_send_binary(state->socket, (void*)message.data(), message.size());
    #else
        state->socket.sendText(message);
    #endif
}

void NetworkManager::OnDisconnectedFromServer() {
    connected = false;
}

void NetworkManager::OnMessageRecieved(const std::string& msg) {

}

void NetworkManager::OnError(const std::string& msg) {
    std::cout << "Network Error: " << msg << std::endl;
}

void NetworkManager::Initialize() {
#ifdef __EMSCRIPTEN__
    std::cout << "Initializing emscripten networking backend..." << std::endl;
#else
    ix::initNetSystem();
#endif

    std::cout << "Network backend initialized successfully" << std::endl;
}

void NetworkManager::ConnectToServer() {
    state = std::make_unique<NetworkManagerState>();

#ifdef __EMSCRIPTEN__
    EmscriptenWebSocketCreateAttributes attrs;
    emscripten_websocket_init_create_attributes(&attrs);
    attrs.url = SERVER_URL;
    attrs.protocols = nullptr;
    attrs.createOnMainThread = true;
    state->socket = emscripten_websocket_new(&attrs);

    emscripten_websocket_set_onopen_callback(state->socket, nullptr, NetworkManager::OnEMOpen);
    emscripten_websocket_set_onclose_callback(state->socket, nullptr, NetworkManager::OnEMClose);
    emscripten_websocket_set_onmessage_callback(state->socket, nullptr, NetworkManager::OnEMMessage);
    emscripten_websocket_set_onerror_callback(state->socket, nullptr, NetworkManager::OnEMError);
#else
    state->socket.setUrl(SERVER_URL);
    state->socket.setPingInterval(HEARTBEAT_PING_INTERVAL);
    state->socket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg) {
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
    state->socket.send("hello world!");
#endif
}

void NetworkManager::CreateLobby() {

}

void NetworkManager::JoinLobby(int lobbyCode) {

}

void NetworkManager::Shutdown() {
#ifndef __EMSCRIPTEN__
    state->socket.stop();
    ix::uninitNetSystem();
#endif
    std::cout << "Network backend shutdown successfully" << std::endl;
}
