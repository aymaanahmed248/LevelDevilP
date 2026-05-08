#pragma once
// ============================================================
//  Constants.h  -  Global game configuration
// ============================================================
#include "raylib.h"

// Window
constexpr int   SCREEN_W        = 1280;
constexpr int   SCREEN_H        = 720;
constexpr char  WINDOW_TITLE[]  = "Trick & Trap";
constexpr int   TARGET_FPS      = 60;

// Player physics
constexpr float PLAYER_SPEED      = 220.0f;
constexpr float PLAYER_JUMP_FORCE = -520.0f;
constexpr float GRAVITY           = 1100.0f;
constexpr float MAX_FALL_SPEED    = 900.0f;
constexpr float PLAYER_W          = 36.0f;
constexpr float PLAYER_H          = 48.0f;

// Tiles
constexpr float TILE_SIZE = 48.0f;

// Death flash duration in seconds
constexpr float DEATH_FLASH_DURATION = 0.25f;

// Leaderboard max entries
constexpr int MAX_SCORES = 5;

// Colors
inline constexpr Color COL_BG_1         = {30,  30,  46,  255};
inline constexpr Color COL_BG_2         = {24,  24,  37,  255};
inline constexpr Color COL_PLATFORM     = {137, 180, 130, 255};
inline constexpr Color COL_PLAYER       = {137, 220, 235, 255};
inline constexpr Color COL_SPIKE        = {243, 139, 168, 255};
inline constexpr Color COL_MOVING_PLAT  = {250, 179, 135, 255};
inline constexpr Color COL_FALLING_PLAT = {203, 166, 247, 255};
inline constexpr Color COL_GOAL         = {166, 227, 161, 255};
inline constexpr Color COL_UI_TEXT      = {205, 214, 244, 255};
inline constexpr Color COL_DEATH_FLASH  = {243, 139, 168, 100};
