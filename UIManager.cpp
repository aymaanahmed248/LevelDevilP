// ============================================================
//  UIManager.cpp
// ============================================================
#include "UIManager.h"
#include <cmath>
#include <stdexcept>

UIManager::UIManager() {
    m_settings["musicVolume"] = 3;
    m_settings["sfxVolume"]   = 5;
}

// Private helper: draw a clickable button, returns true if clicked
bool UIManager::drawButton(int cx, int cy, int w, int h,
                            const std::string& label, Color col) {
    Rectangle rect = {
        (float)(cx - w / 2), (float)(cy - h / 2),
        (float)w,            (float)h
    };
    Vector2 mouse   = GetMousePosition();
    bool    hovered = CheckCollisionPointRec(mouse, rect);

    Color bg = hovered ? ColorBrightness(col, 0.3f) : col;
    DrawRectangleRec(rect, bg);
    DrawRectangleLinesEx(rect, 2, WHITE);

    int fontSize = 22;
    int tw = MeasureText(label.c_str(), fontSize);
    DrawText(label.c_str(), cx - tw / 2, cy - fontSize / 2, fontSize, WHITE);

    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void UIManager::drawStartMenu(std::function<void()> onPlay,
                               std::function<void()> onQuit) {
    DrawRectangleGradientV(0, 0, SCREEN_W, SCREEN_H, COL_BG_1, COL_BG_2);

    // Pulsing title
    float scale = 1.0f + 0.03f * sinf((float)GetTime() * 2.0f);
    const char* title = "TRICK & TRAP";
    int tSize = (int)(52 * scale);
    int tw    = MeasureText(title, tSize);
    DrawText(title, SCREEN_W / 2 - tw / 2, 140, tSize, COL_GOAL);

    DrawText("A Troll Platformer",
             SCREEN_W / 2 - 110, 215, 24, COL_UI_TEXT);
    DrawText("A/D or Arrow Keys to move   |   SPACE to jump",
             SCREEN_W / 2 - 230, 260, 20, GRAY);

    if (drawButton(SCREEN_W / 2, 370, 220, 55, "PLAY", {100, 180, 100, 255}))
        onPlay();

    if (drawButton(SCREEN_W / 2, 445, 220, 55, "QUIT", {180, 80, 80, 255}))
        onQuit();

    DrawText("v1.0  |  CSE142 OOP Project", 10, SCREEN_H - 24, 16, DARKGRAY);
}

void UIManager::drawHUD(int currentLevel, int deaths, bool reverseActive) {
    DrawText(TextFormat("Level %d / 4", currentLevel), 14, 14, 22, COL_UI_TEXT);
    DrawText(TextFormat("Deaths: %d",   deaths),       14, 42, 20, COL_UI_TEXT);

    if (reverseActive) {
        float    pulse = 0.5f + 0.5f * sinf((float)GetTime() * 8.0f);
        Color    wCol  = {255, (unsigned char)(80 * pulse), 80, 255};
        const char* msg = "CONTROLS REVERSED!";
        int tw = MeasureText(msg, 28);
        DrawText(msg, SCREEN_W / 2 - tw / 2, 14, 28, wCol);
    }

    DrawText("A/D: Move   SPACE: Jump", 14, SCREEN_H - 28, 16, DARKGRAY);
}

void UIManager::drawDeathScreen(int deaths, std::function<void()> onRetry) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 160});

    const char* msg = "YOU DIED";
    int tw = MeasureText(msg, 60);
    DrawText(msg, SCREEN_W / 2 - tw / 2, SCREEN_H / 2 - 110, 60, COL_SPIKE);
    DrawText(TextFormat("Deaths so far: %d", deaths),
             SCREEN_W / 2 - 100, SCREEN_H / 2 - 25, 24, COL_UI_TEXT);
    DrawText("(respawning in 1s...)",
             SCREEN_W / 2 - 110, SCREEN_H / 2 + 15, 20, GRAY);

    if (drawButton(SCREEN_W / 2, SCREEN_H / 2 + 100, 200, 50,
                   "RETRY NOW", {180, 80, 80, 255}))
        onRetry();
}

void UIManager::drawWinScreen(int level, int deaths,
                               std::function<void()> onNext,
                               std::function<void()> onMenu,
                               bool isFinalLevel) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 180});

    const char* msg = isFinalLevel ? "YOU WIN!!!" : "LEVEL CLEAR!";
    int tw = MeasureText(msg, 56);
    DrawText(msg, SCREEN_W / 2 - tw / 2, SCREEN_H / 2 - 140, 56, COL_GOAL);

    DrawText(TextFormat("Level %d complete", level),
             SCREEN_W / 2 - 110, SCREEN_H / 2 - 60, 26, COL_UI_TEXT);
    DrawText(TextFormat("Deaths: %d", deaths),
             SCREEN_W / 2 - 60,  SCREEN_H / 2 - 20, 22, COL_UI_TEXT);

    if (isFinalLevel) {
        DrawText("Congratulations! You survived all 4 levels.",
                 SCREEN_W / 2 - 245, SCREEN_H / 2 + 30, 22, GOLD);
    } else {
        if (drawButton(SCREEN_W / 2, SCREEN_H / 2 + 80, 220, 50,
                       "NEXT LEVEL", {80, 160, 80, 255}))
            onNext();
    }

    if (drawButton(SCREEN_W / 2, SCREEN_H / 2 + 150, 220, 50,
                   "MAIN MENU", {100, 100, 180, 255}))
        onMenu();
}

void UIManager::drawLeaderboard(const std::vector<ScoreEntry>& scores) {
    DrawText("--- Leaderboard ---", SCREEN_W - 270, 14, 20, GOLD);
    auto sorted = scores;
    sortAscending(sorted);  // uses the template function

    int y = 44;
    for (int i = 0; i < (int)sorted.size() && i < MAX_SCORES; ++i) {
        DrawText(TextFormat("%d. %s  Deaths:%d  Lvl%d",
                            i + 1,
                            sorted[i].playerName.c_str(),
                            sorted[i].deaths,
                            sorted[i].level),
                 SCREEN_W - 380, y, 16, COL_UI_TEXT);
        y += 22;
    }
}

void UIManager::drawLevelTransition(int newLevel) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 140});
    const char* msg = TextFormat("Level %d", newLevel);
    int tw = MeasureText(msg, 52);
    DrawText(msg, SCREEN_W / 2 - tw / 2, SCREEN_H / 2 - 30, 52, COL_GOAL);
}

void UIManager::addScore(const std::string& name, int deaths, int level) {
    if (name.empty()) throw std::invalid_argument("Player name cannot be empty");
    if (deaths < 0)   throw std::invalid_argument("Deaths cannot be negative");

    m_scores.push_back({name, deaths, level});
    sortAscending(m_scores);
    if ((int)m_scores.size() > MAX_SCORES)
        m_scores.resize(MAX_SCORES);
}

int UIManager::getSetting(const std::string& key, int def) const {
    auto it = m_settings.find(key);
    return (it != m_settings.end()) ? it->second : def;
}
