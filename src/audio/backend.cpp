#define MINIAUDIO_IMPLEMENTATION
#include "../graphics/loader.hpp"
#include "backend.hpp"

#include <iostream>

void AudioBackend::Initialize() {
    audioResult = ma_engine_init(NULL, &audioEngine);
    if (audioResult != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize audio backend");
    }
    std::cout << "Audio backend initialized successfully" << std::endl;

    Loader::LoadSoundFromFile("resources/audio/glass_006.wav", globalSounds.buttonClick);
    Loader::LoadSoundFromFile("resources/audio/glitch_004.wav", globalSounds.hover);
}

void AudioBackend::PlayAudio(const std::string& resourcePath, float volume, float pitchFactor) {
    ma_engine_play_sound(&audioEngine, resourcePath.c_str(), NULL);
    ma_engine_set_volume(&audioEngine, volume);
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

void AudioBackend::UnloadSoundAsset(Sound &sound) {
    ma_sound_uninit(&sound.value);
}

void AudioBackend::Shutdown() {
    UnloadSoundAsset(globalSounds.buttonClick);
    UnloadSoundAsset(globalSounds.hover);

    ma_engine_uninit(&audioEngine);
}
