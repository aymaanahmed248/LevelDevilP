#pragma once
// ============================================================
//  UIManager.h  -  HUD, menus, leaderboard
//
//  OOP: template function sortAscending<T>,
//       STL map for settings, lambdas for callbacks
// ============================================================
#include "raylib.h"
#include "Constants.h"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>

// Score entry for leaderboard
struct ScoreEntry {
    std::string playerName;
    int         deaths;
    int         level;

    // Operator overloading: lower deaths = better
    bool operator<(const ScoreEntry& o) const { return deaths < o.deaths; }
};

// Template function: sort any vector of comparable elements
template<typename T>
void sortAscending(std::vector<T>& vec) {
    std::sort(vec.begin(), vec.end());
}

class UIManager {
public:
    UIManager();
    ~UIManager() = default;

    void drawStartMenu(std::function<void()> onPlay,
                       std::function<void()> onQuit);

    void drawHUD(int currentLevel, int deaths, bool reverseActive);

    void drawDeathScreen(int deaths, std::function<void()> onRetry);

    void drawWinScreen(int level, int deaths,
                       std::function<void()> onNext,
                       std::function<void()> onMenu,
                       bool isFinalLevel);

    void drawLeaderboard(const std::vector<ScoreEntry>& scores);
    void drawLevelTransition(int newLevel);

    void addScore(const std::string& name, int deaths, int level);
    const std::vector<ScoreEntry>& getScores() const { return m_scores; }

    void setSetting(const std::string& key, int value) { m_settings[key] = value; }
    int  getSetting(const std::string& key, int def = 0) const;

private:
    bool drawButton(int cx, int cy, int w, int h,
                    const std::string& label, Color col);

    std::vector<ScoreEntry>    m_scores;
    std::map<std::string, int> m_settings;
};
