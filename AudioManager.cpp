// ============================================================
//  AudioManager.cpp  -  Procedural sound generation
// ============================================================
#include "AudioManager.h"
#include <cmath>

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

// Generate a sine-wave beep as a Raylib Sound
Sound AudioManager::makeBeep(float freq, float duration, float volume) {
    const int sampleRate = 44100;
    int       samples    = (int)(sampleRate * duration);

    Wave wave        = {};
    wave.frameCount  = (unsigned int)samples;
    wave.sampleRate  = (unsigned int)sampleRate;
    wave.sampleSize  = 16;
    wave.channels    = 1;

    short* data = new short[samples];
    for (int i = 0; i < samples; ++i) {
        float t        = (float)i / (float)sampleRate;
        float envelope = 1.0f - (t / duration);  // linear fade out
        data[i] = (short)(sinf(2.0f * 3.14159265f * freq * t)
                          * envelope * 32760.0f * volume);
    }
    wave.data = data;

    Sound sound = LoadSoundFromWave(wave);
    delete[] data;
    return sound;
}

void AudioManager::init() {
    if (m_initialised) return;
    InitAudioDevice();

    m_sounds["jump"]    = makeBeep(440.0f, 0.12f, 0.5f);
    m_sounds["death"]   = makeBeep(180.0f, 0.35f, 0.6f);
    m_sounds["win"]     = makeBeep(880.0f, 0.40f, 0.5f);
    m_sounds["land"]    = makeBeep(220.0f, 0.08f, 0.3f);
    m_sounds["reverse"] = makeBeep(600.0f, 0.30f, 0.4f);

    m_initialised = true;
}

void AudioManager::shutdown() {
    for (auto& [name, snd] : m_sounds) UnloadSound(snd);
    CloseAudioDevice();
    m_initialised = false;
}

void AudioManager::playSound(const std::string& name) {
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        SetSoundVolume(it->second, m_musicVolume * 1.5f);
        PlaySound(it->second);
    }
}

// Simple chiptune: plays a looping pentatonic melody
void AudioManager::updateMusic() {
    static const float notes[] = {
        261.6f, 293.6f, 329.6f, 392.0f,
        440.0f, 392.0f, 329.6f, 293.6f
    };
    static int   noteIdx  = 0;
    static float noteBeat = 0.0f;
    static Sound bgNote   = {};
    static bool  bgInit   = false;

    noteBeat += GetFrameTime();

    constexpr float BPM      = 140.0f;
    constexpr float BEAT_DUR = 60.0f / BPM / 2.0f;  // eighth notes

    if (noteBeat >= BEAT_DUR) {
        noteBeat = 0;
        if (bgInit) {
            StopSound(bgNote);
            UnloadSound(bgNote);
        }
        bgNote = makeBeep(notes[noteIdx % 8], BEAT_DUR * 0.85f, m_musicVolume);
        PlaySound(bgNote);
        bgInit = true;
        ++noteIdx;
    }
}

void AudioManager::setMusicVolume(float v) {
    m_musicVolume = v;
}
