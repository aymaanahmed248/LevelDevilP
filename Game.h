#pragma once
// ============================================================
//  Game.h  -  Top-level state machine
// ============================================================
#include "Level.h"
#include "UIManager.h"
#include "AudioManager.h"
#include <memory>

enum class GameState {
    MENU,
    TRANSITION,
    PLAYING,
    DEAD,
    LEVEL_WIN,
    GAME_WIN
};

class Game {
public:
    Game();
    ~Game();

    void update();
    void draw();
    bool shouldClose() const { return m_shouldClose; }

private:
    void startLevel(int num);
    void onPlayerDeath();
    void onLevelComplete();

    void updateMenu();
    void updateTransition();
    void updatePlaying();
    void updateDead();
    void updateLevelWin();

    void drawMenu();
    void drawTransition();
    void drawPlaying();
    void drawDead();
    void drawLevelWin();
    void drawGameWin();

    std::unique_ptr<Level> m_level;
    UIManager              m_ui;
    GameState              m_state;
    int                    m_currentLevel;
    int                    m_totalDeaths;
    float                  m_deadTimer;
    float                  m_transitionTimer;
    bool                   m_shouldClose;
};
