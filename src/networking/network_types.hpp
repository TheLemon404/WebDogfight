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

class Packet {
    int readOffset = 0;
    std::string buffer;

    public:
    Packet(const std::string& buffer) : buffer(buffer) {};
    Packet() {};
    
    Packet& WritePacketType(PacketType val) {
        buffer.push_back((char)val);
        return *this;
    }

    Packet& WriteU8(uint8_t val) {
        buffer.push_back((char)val);
        return *this;
    }

    Packet& WriteU16(uint16_t val) {
        buffer.push_back((char)((val >> 8) & 0xFF));
        buffer.push_back((char)(val & 0xFF));
        return *this;
    }

    Packet& WriteU32(uint32_t val) {
        buffer.push_back((char)((val >> 24) & 0xFF));
        buffer.push_back((char)((val >> 16) & 0xFF));
        buffer.push_back((char)((val >> 8) & 0xFF));
        buffer.push_back((char)(val & 0xFF));
        return *this;
    }

    Packet& WriteF32(float val) {
        uint32_t ival = (uint32_t)val;
        buffer.push_back((char)((ival >> 24) & 0xFF));
        buffer.push_back((char)((ival >> 16) & 0xFF));
        buffer.push_back((char)((ival >> 8) & 0xFF));
        buffer.push_back((char)(ival & 0xFF));
        return *this;
    }

    PacketType ReadPacketType() {
        return (PacketType)buffer[readOffset++];
    }

    uint8_t ReadU8() {
        return (uint8_t)buffer[readOffset++];
    }

    uint16_t ReadU16() {
        return (uint16_t)(((uint8_t)buffer[readOffset++] << 8) | (uint8_t)buffer[readOffset++]);
    }

    uint32_t ReadU32() {
        return (uint32_t)(((uint8_t)buffer[readOffset++] << 24) | ((uint8_t)buffer[readOffset++] << 16) | ((uint8_t)buffer[readOffset++] << 8) | (uint8_t)buffer[readOffset++]);
    }

    float ReadF32() {
        return (float)(((uint8_t)buffer[readOffset++] << 24) | ((uint8_t)buffer[readOffset++] << 16) | ((uint8_t)buffer[readOffset++] << 8) | (uint8_t)buffer[readOffset++]);
    }

    const std::string& Build() {
        return buffer;
    }
};
