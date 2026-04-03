#pragma once

#include <miniaudio.h>
#include <string>

struct Sound {
    ma_sound value;
};

struct GlobalSounds {
    Sound buttonClick;
    Sound hover;
};

class AudioBackend{
    ma_result audioResult;

    public:
    GlobalSounds globalSounds;
    ma_engine audioEngine;

    void Initialize();

    void PlayAudio(const std::string& resourcePath, float volume = 1.0f, float pitchFactor = 1.0f);

    void StartSoundAsset(Sound& sound, bool looping = false, float volume = 1.0f);
    void SoundAssetSetVolume(Sound& sound, float volume);
    void SoundAssetSetPitch(Sound& sound, float pitch);
    void EndSoundAsset(Sound& sound);
    void UnloadSoundAsset(Sound& sound);

    void Shutdown();
};
