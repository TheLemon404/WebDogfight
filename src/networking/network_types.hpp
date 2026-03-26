#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cstdint>
#include <string>

enum class PacketType : uint8_t {
    //request types
    JOIN_RANDOM_LOBBY = 0x00,
    JOIN_LOBBY = 0x01,
    CREATE_LOBBY = 0x02,
    LEAVE_LOBBY = 0x03,

    UPDATE_CLIENT_STATE = 0x04,

    RPC = 0x05,

    //response types
    CONNECTED_TO_SERVER = 0x06,
    LOBBY_JOINED = 0x07,
    LOBBY_LEFT = 0x08,

    LOBBY_STATE_UPDATED = 0x09,
};

class Packet {
    int readOffset = 0;
    std::string buffer;

    public:
    Packet(const std::string& buffer) : buffer(buffer) {};
    Packet() {};

    void Rewind() {
        readOffset = 0;
    }

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
        uint32_t ival = std::bit_cast<uint32_t>(val);
        buffer.push_back((char)((ival >> 24) & 0xFF));
        buffer.push_back((char)((ival >> 16) & 0xFF));
        buffer.push_back((char)((ival >> 8) & 0xFF));
        buffer.push_back((char)(ival & 0xFF));
        return *this;
    }

    Packet& WriteBuffer(std::string val) {
        buffer += val;
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
        uint32_t ival = (uint32_t)(((uint8_t)buffer[readOffset++] << 24) | ((uint8_t)buffer[readOffset++] << 16) | ((uint8_t)buffer[readOffset++] << 8) | (uint8_t)buffer[readOffset++]);
        return std::bit_cast<float>(ival);
    }

    const std::string& Build() {
        return buffer;
    }
};
