// ============================================================
//  Game.cpp  -  State machine
// ============================================================
#include "Game.h"
#include "Constants.h"
#include "raylib.h"
#include <stdexcept>
#include <iostream>

Game::Game()
    : m_state(GameState::MENU),
      m_currentLevel(1),
      m_totalDeaths(0),
      m_deadTimer(0),
      m_transitionTimer(0),
      m_shouldClose(false)
{
    InitWindow(SCREEN_W, SCREEN_H, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);
    SetExitKey(KEY_NULL);

    AudioManager::getInstance().init();
}

Game::~Game() {
    AudioManager::getInstance().shutdown();
    CloseWindow();
}

void Game::update() {
    if (WindowShouldClose()) { m_shouldClose = true; return; }

    AudioManager::getInstance().updateMusic();

    switch (m_state) {
        case GameState::MENU:       updateMenu();       break;
        case GameState::TRANSITION: updateTransition(); break;
        case GameState::PLAYING:    updatePlaying();    break;
        case GameState::DEAD:       updateDead();       break;
        case GameState::LEVEL_WIN:  updateLevelWin();   break;
        case GameState::GAME_WIN:                       break;
    }
}

void Game::draw() {
    BeginDrawing();
    ClearBackground(COL_BG_2);

    switch (m_state) {
        case GameState::MENU:       drawMenu();       break;
        case GameState::TRANSITION: drawTransition(); break;
        case GameState::PLAYING:    drawPlaying();    break;
        case GameState::DEAD:       drawDead();       break;
        case GameState::LEVEL_WIN:  drawLevelWin();   break;
        case GameState::GAME_WIN:   drawGameWin();    break;
    }

    EndDrawing();
}

void Game::startLevel(int num) {
    try {
        m_level = std::make_unique<Level>(num);
        m_currentLevel = num;
    } catch (const std::exception& e) {
        std::cerr << "[Game] Failed to load level " << num
                  << ": " << e.what() << std::endl;
        m_level = std::make_unique<Level>(1);
        m_currentLevel = 1;
    }
}

// ---- MENU ----
void Game::updateMenu() {}

void Game::drawMenu() {
    m_ui.drawStartMenu(
        [this]() {
            m_totalDeaths     = 0;
            m_currentLevel    = 1;
            m_transitionTimer = 1.5f;
            startLevel(1);
            m_state = GameState::TRANSITION;
        },
        [this]() { m_shouldClose = true; }
    );
}

// ---- TRANSITION ----
void Game::updateTransition() {
    m_transitionTimer -= GetFrameTime();
    if (m_transitionTimer <= 0)
        m_state = GameState::PLAYING;
}

void Game::drawTransition() {
    if (m_level) {
        ClearBackground(m_level->getBgColor());
        m_level->draw();
    }
    m_ui.drawLevelTransition(m_currentLevel);
}

// ---- PLAYING ----
void Game::updatePlaying() {
    if (!m_level) return;

    m_level->update(GetFrameTime());

    if (m_level->playerDead())
        onPlayerDeath();
    else if (m_level->isComplete())
        onLevelComplete();
}

void Game::drawPlaying() {
    if (!m_level) return;
    ClearBackground(m_level->getBgColor());
    m_level->draw();

    // FIX: use public getter isReverseControls() instead of private member
    bool reverseActive = m_level->getPlayer().isReverseControls();
    m_ui.drawHUD(m_currentLevel,
                 m_level->getPlayer().getDeaths() + m_totalDeaths,
                 reverseActive);

    if (!m_ui.getScores().empty())
        m_ui.drawLeaderboard(m_ui.getScores());
}

// ---- DEAD ----
void Game::onPlayerDeath() {
    m_totalDeaths++;
    m_deadTimer = 1.0f;
    m_state     = GameState::DEAD;
}

void Game::updateDead() {
    m_deadTimer -= GetFrameTime();
    if (m_deadTimer <= 0) {
        m_level->reset();
        m_state = GameState::PLAYING;
    }
}

void Game::drawDead() {
    if (m_level) {
        ClearBackground(m_level->getBgColor());
        m_level->draw();
    }
    m_ui.drawDeathScreen(
        m_totalDeaths,
        [this]() {
            m_level->reset();
            m_state = GameState::PLAYING;
        }
    );
}

// ---- LEVEL WIN ----
void Game::onLevelComplete() {
    try {
        m_ui.addScore("Player", m_totalDeaths, m_currentLevel);
    } catch (const std::exception& e) {
        std::cerr << "[Game] Score error: " << e.what() << std::endl;
    }
    m_state = GameState::LEVEL_WIN;
}

void Game::updateLevelWin() {}

void Game::drawLevelWin() {
    if (m_level) {
        ClearBackground(m_level->getBgColor());
        m_level->draw();
    }
    bool isFinal = (m_currentLevel == 4);
    m_ui.drawWinScreen(
        m_currentLevel, m_totalDeaths,
        [this]() {
            int next = m_currentLevel + 1;
            if (next > 4) {
                m_state = GameState::GAME_WIN;
            } else {
                m_transitionTimer = 1.5f;
                startLevel(next);
                m_state = GameState::TRANSITION;
            }
        },
        [this]() {
            m_state = GameState::MENU;
            m_level.reset();
        },
        isFinal
    );
}

// ---- GAME WIN ----
void Game::drawGameWin() {
    DrawRectangleGradientV(0, 0, SCREEN_W, SCREEN_H,
                           {10, 10, 30, 255}, {30, 10, 10, 255});

    const char* msg = "YOU BEAT TRICK & TRAP!";
    int tw = MeasureText(msg, 48);
    DrawText(msg, SCREEN_W / 2 - tw / 2, 180, 48, GOLD);
    DrawText("Thanks for playing!", SCREEN_W / 2 - 110, 250, 28, COL_UI_TEXT);
    DrawText(TextFormat("Total deaths: %d", m_totalDeaths),
             SCREEN_W / 2 - 90, 300, 24, COL_UI_TEXT);

    m_ui.drawLeaderboard(m_ui.getScores());

    Rectangle btn = {(float)(SCREEN_W / 2 - 110), 420, 220, 55};
    Vector2   mouse = GetMousePosition();
    bool      hov   = CheckCollisionPointRec(mouse, btn);
    DrawRectangleRec(btn, hov ? DARKBLUE : BLUE);
    int bw = MeasureText("MAIN MENU", 22);
    DrawText("MAIN MENU", SCREEN_W / 2 - bw / 2, 438, 22, WHITE);

    if (hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_state        = GameState::MENU;
        m_totalDeaths  = 0;
        m_currentLevel = 1;
        m_level.reset();
    }
}
