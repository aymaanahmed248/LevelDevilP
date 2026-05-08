// ============================================================
//  Player.cpp
// ============================================================
#include "Player.h"
#include "Constants.h"
#include "raylib.h"
#include <vector>
#include <algorithm>

Player::Player(float spawnX, float spawnY)
    : Entity(spawnX, spawnY, PLAYER_W, PLAYER_H),
      m_vx(0), m_vy(0),
      m_spawnX(spawnX), m_spawnY(spawnY),
      m_onGround(false),
      m_dead(false), m_won(false),
      m_reverseControls(false),
      m_deaths(0),
      m_deathFlashTimer(0.0f)
{}

// Copy constructor
Player::Player(const Player& other)
    : Entity(other.m_x, other.m_y, other.m_w, other.m_h),
      m_vx(other.m_vx), m_vy(other.m_vy),
      m_spawnX(other.m_spawnX), m_spawnY(other.m_spawnY),
      m_onGround(other.m_onGround),
      m_dead(other.m_dead), m_won(other.m_won),
      m_reverseControls(other.m_reverseControls),
      m_deaths(other.m_deaths),
      m_deathFlashTimer(0.0f)
{}

void Player::handleInput(float dt) {
    if (m_dead || m_won) return;

    // Reverse controls is a troll mechanic - flips left/right
    float dir = m_reverseControls ? -1.0f : 1.0f;

    m_vx = 0.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  m_vx = -PLAYER_SPEED * dir;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) m_vx =  PLAYER_SPEED * dir;

    // Jump only when on ground
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        && m_onGround)
    {
        m_vy = PLAYER_JUMP_FORCE;
        m_onGround = false;
    }
}

void Player::applyGravity(float dt) {
    if (m_dead || m_won) return;
    m_vy += GRAVITY * dt;
    if (m_vy > MAX_FALL_SPEED) m_vy = MAX_FALL_SPEED;
}

void Player::resolveCollisions(const std::vector<Rectangle>& platforms) {
    if (m_dead || m_won) return;

    m_onGround = false;
    float dt = GetFrameTime();

    // Horizontal pass
    m_x += m_vx * dt;
    for (const auto& p : platforms) {
        if (CheckCollisionRecs(getRect(), p)) {
            if (m_vx > 0) m_x = p.x - m_w;
            else           m_x = p.x + p.width;
            m_vx = 0;
        }
    }

    // Vertical pass
    m_y += m_vy * dt;
    for (const auto& p : platforms) {
        if (CheckCollisionRecs(getRect(), p)) {
            if (m_vy > 0) {
                m_y = p.y - m_h;
                m_onGround = true;
            } else {
                m_y = p.y + p.height;
            }
            m_vy = 0;
        }
    }
}

void Player::update(float dt) {
    if (m_deathFlashTimer > 0) m_deathFlashTimer -= dt;

    // Kill player if they fall off the bottom of the screen
    if (m_y > SCREEN_H + 200) {
        m_dead = true;
    }
}

void Player::draw() const {
    // Red screen flash on death
    if (m_deathFlashTimer > 0) {
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, COL_DEATH_FLASH);
    }

    Color bodyColor = m_reverseControls ? ORANGE : COL_PLAYER;

    // Body
    DrawRectangleRec(getRect(), bodyColor);

    // Eyes - shift based on movement direction
    float eyeOff = (m_vx < 0) ? 4.0f : 18.0f;
    float eyeY   = m_y + 12;
    DrawCircle((int)(m_x + eyeOff),      (int)eyeY, 5, WHITE);
    DrawCircle((int)(m_x + eyeOff + 10), (int)eyeY, 5, WHITE);
    DrawCircle((int)(m_x + eyeOff + 1),  (int)eyeY, 2, DARKGRAY);
    DrawCircle((int)(m_x + eyeOff + 11), (int)eyeY, 2, DARKGRAY);

    // Mouth
    if (!m_reverseControls)
        DrawCircleLines((int)(m_x + m_w / 2), (int)(m_y + 32), 7, DARKGRAY);
    else
        DrawText(":(", (int)(m_x + 6), (int)(m_y + 28), 14, DARKGRAY);

    // Outline
    DrawRectangleLinesEx(getRect(), 2, DARKGRAY);
}

void Player::respawn() {
    m_x  = m_spawnX;
    m_y  = m_spawnY;
    m_vx = 0;
    m_vy = 0;
    m_dead            = false;
    m_won             = false;
    m_onGround        = false;
    m_reverseControls = false;
    m_deathFlashTimer = DEATH_FLASH_DURATION;
}
