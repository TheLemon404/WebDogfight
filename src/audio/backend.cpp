#define MINIAUDIO_IMPLEMENTATION
#include "backend.hpp"

#include <iostream>

void AudioBackend::Initialize() {
    audioResult = ma_engine_init(NULL, &audioEngine);
    if (audioResult != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize audio backend");
    }
    std::cout << "Audio backend initialized successfully" << std::endl;
}

void AudioBackend::PlayAudio(const std::string& resourcePath, float volume, float pitchFactor) {
    ma_engine_play_sound(&audioEngine, resourcePath.c_str(), NULL);
    ma_engine_set_volume(&audioEngine, volume);
}

void AudioBackend::LoadSound(const std::string& resourcePath, Sound& sound) {
    std::cout << "Attempting to loaded sound file from: " << resourcePath << std::endl;

    int loadResult = ma_sound_init_from_file(&audioEngine, resourcePath.c_str(), 0, nullptr, nullptr, &sound.value);
    if(loadResult != MA_SUCCESS) {
        throw std::runtime_error("Failed to load sound asset");
    }
}

void AudioBackend::StartSoundAsset(Sound& sound, bool looping, float volume) {
    ma_sound_start(&sound.value);

    ma_sound_set_looping(&sound.value, looping ? MA_TRUE : MA_FALSE);
    ma_sound_set_volume(&sound.value, volume);
}

void AudioBackend::SoundAssetSetVolume(Sound& sound, float volume) {
    ma_sound_set_volume(&sound.value, volume);
}

void AudioBackend::SoundAssetSetPitch(Sound& sound, float pitch) {
    ma_sound_set_pitch(&sound.value, pitch);
}

void AudioBackend::EndSoundAsset(Sound& sound) {
    ma_sound_stop(&sound.value);
}

void AudioBackend::Shutdown() {
    ma_engine_uninit(&audioEngine);
}
