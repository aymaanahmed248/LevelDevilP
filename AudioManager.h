#pragma once
// ============================================================
//  AudioManager.h  -  Procedural audio using Raylib
//  Singleton pattern - access via getInstance()
// ============================================================
#include "raylib.h"
#include <map>
#include <string>

class AudioManager {
public:
    static AudioManager& getInstance();

    void init();
    void shutdown();
    void playSound(const std::string& name);
    void updateMusic();
    void setMusicVolume(float v);

    // No copy or assign for singleton
    AudioManager(const AudioManager&)            = delete;
    AudioManager& operator=(const AudioManager&) = delete;

private:
    AudioManager()  = default;
    ~AudioManager() = default;

    Sound makeBeep(float freq, float duration, float volume = 0.4f);

    std::map<std::string, Sound> m_sounds;
    bool  m_initialised = false;
    float m_musicVolume = 0.3f;
};
