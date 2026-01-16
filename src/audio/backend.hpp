#pragma once

#include <miniaudio.h>
#include <string>

struct Sound {
    ma_sound value;
};

class AudioBackend{
    inline static ma_result audioResult;
    inline static ma_engine audioEngine;

    public:
    static void Initialize();

    static void PlayAudio(const std::string& resourcePath, float volume = 1.0f, float pitchFactor = 1.0f);

    static void LoadSound(const std::string& resourcePath, Sound& sound);
    static void StartSoundAsset(Sound& sound, bool looping = false, float volume = 1.0f);
    static void SoundAssetSetVolume(Sound& sound, float volume);
    static void SoundAssetSetPitch(Sound& sound, float pitch);
    static void EndSoundAsset(Sound& sound);

    static void Shutdown();
};
