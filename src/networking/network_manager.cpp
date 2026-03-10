#include "network_manager.hpp"

#ifndef __EMSCRIPTEN__
#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocketMessageType.h"
#endif

#include <iostream>

#define SERVER_URL "ws://127.0.0.1:1234/"
#define HEARTBEAT_PING_INTERVAL 45

#ifdef __EMSCRIPTEN__
EM_BOOL NetworkManager::OnEMOpen(int type, const EmscriptenWebSocketOpenEvent* e, void* ud) {
    std::cout << "Connected to server (emscripten)" << std::endl;
    OnConnectedToServer();

    return EM_TRUE;
}

EM_BOOL NetworkManager::OnEMClose(int type, const EmscriptenWebSocketCloseEvent* e, void* ud) {
    std::cout << "Disconnected from server (emscripten)" << std::endl;
    connected = false;
    return EM_TRUE;
}

EM_BOOL NetworkManager::OnEMMessage(int type, const EmscriptenWebSocketMessageEvent* e, void* ud) {
    if(e->isText) return EM_TRUE;
    std::string data(e->data, e->data + e->numBytes);
    OnMessageRecieved(data);
    return EM_TRUE;
}

EM_BOOL NetworkManager::OnEMError(int type, const EmscriptenWebSocketErrorEvent* e, void* ud) {
    std::cerr << "WebSocket error (emscripten)" << std::endl;
    return EM_TRUE;
}
#endif

void NetworkManager::OnConnectedToServer() {
    const std::string message = "jr";
#ifdef __EMSCRIPTEN__
    emscripten_websocket_send_utf8_text(state->socket, message);
#else
    state->socket.sendText(message);
#endif

    connected = true;
}

void NetworkManager::OnMessageRecieved(const std::string& msg) {
    std::cout << "message recieved" << std::endl;
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
                std::cout << "Connected to server (ixwebsocket)" << std::endl;
                OnConnectedToServer();
                break;
            case ix::WebSocketMessageType::Close:
                std::cout << "Disconnected from server (ixwebsocket)" << std::endl;
                connected = false;
                break;
            case ix::WebSocketMessageType::Message:
                OnMessageRecieved(msg->str);
                break;
            case ix::WebSocketMessageType::Error:
                std::cerr << "WebSocket error (ixwebsocket) reason: " << msg->errorInfo.reason << std::endl;
                break;
        }
    });

    std::cout << "websocket created and connected to server" << std::endl;

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
