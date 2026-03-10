#pragma once

#include <cstdint>

enum class PacketType : uint8_t {
    //request types
    JOIN_RANDOM_LOBBY = 0x00,
    JOIN_LOBBY = 0x01,
    LEAVE_LOBBY = 0x02,

    UPDATE_STATE = 0x03,

    RPC = 0x04,

    //response types
    LOBBY_JOINED = 0x05,
    LOBBY_LEFT = 0x06,

    STATE_UPDATED = 0x07,
};
