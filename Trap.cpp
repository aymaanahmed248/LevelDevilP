// ============================================================
//  Trap.cpp
// ============================================================
#include "Trap.h"
#include "Player.h"
#include "Constants.h"
#include "raylib.h"
#include <cmath>

// ============================================================
//  Trap (base)
// ============================================================
Trap::Trap(float x, float y, float w, float h)
    : Entity(x, y, w, h), m_lethal(false), m_timer(0.0f)
{}

// ============================================================
//  SpikeTrap
// ============================================================
SpikeTrap::SpikeTrap(float x, float y, float w, float h, float delay)
    : Trap(x, y, w, h),
      m_delay(delay),
      m_initialDelay(delay),
      m_visible(delay <= 0.0f)
{
    m_lethal = m_visible;
}

void SpikeTrap::resetState() {
    // Go back to hidden - crucial so spikes don't appear instantly after death
    m_timer   = 0.0f;
    m_delay   = m_initialDelay;
    m_visible = (m_initialDelay <= 0.0f);
    m_lethal  = m_visible;
}

void SpikeTrap::update(float dt) {
    if (!m_visible) {
        m_timer += dt;
        if (m_timer >= m_delay) {
            m_visible = true;
            m_lethal  = true;
        }
    }
}

void SpikeTrap::draw() const {
    if (!m_visible) return;

    int   count = (int)(m_w / 16);
    if (count < 1) count = 1;
    float sw = m_w / count;
    for (int i = 0; i < count; ++i) {
        float sx = m_x + i * sw;
        DrawTriangle(
            { sx + sw * 0.5f, m_y       },
            { sx,             m_y + m_h },
            { sx + sw,        m_y + m_h },
            COL_SPIKE
        );
    }
    DrawRectangle((int)m_x, (int)(m_y + m_h - 4), (int)m_w, 4, RED);
}

void SpikeTrap::checkPlayerInteraction(Player& player) {
    if (!m_visible || !m_lethal) return;
    if (CheckCollisionRecs(getRect(), player.getRect()))
        player.setDead(true);
}

// ============================================================
//  MovingPlatform
// ============================================================
MovingPlatform::MovingPlatform(float x, float y, float w, float h,
                                float speed, float range, int axis)
    : Trap(x, y, w, h),
      m_speed(speed), m_range(range), m_axis(axis),
      m_origin(axis == 0 ? x : y), m_dir(1)
{
    m_lethal = false;
}

void MovingPlatform::update(float dt) {
    if (m_axis == 0) {
        m_x += m_speed * m_dir * dt;
        if (m_x > m_origin + m_range) { m_x = m_origin + m_range; m_dir = -1; }
        if (m_x < m_origin)           { m_x = m_origin;            m_dir =  1; }
    } else {
        m_y += m_speed * m_dir * dt;
        if (m_y > m_origin + m_range) { m_y = m_origin + m_range; m_dir = -1; }
        if (m_y < m_origin)           { m_y = m_origin;            m_dir =  1; }
    }
}

void MovingPlatform::draw() const {
    DrawRectangleRec(getRect(), COL_MOVING_PLAT);
    DrawRectangleLinesEx(getRect(), 2, ORANGE);
    const char* arrow = (m_axis == 0)
        ? (m_dir > 0 ? ">>>" : "<<<")
        : (m_dir > 0 ? "vvv" : "^^^");
    int tw = MeasureText(arrow, 14);
    DrawText(arrow, (int)(m_x + m_w/2 - tw/2), (int)(m_y + m_h/2 - 7), 14, DARKGRAY);
}

void MovingPlatform::checkPlayerInteraction(Player& /*player*/) {}

void MovingPlatform::carryPlayer(Player& player) {
    if (m_axis == 0) {
        player.setPosition(
            player.getX() + m_speed * m_dir * GetFrameTime(),
            player.getY()
        );
    }
}

// ============================================================
//  FallingPlatform
// ============================================================
FallingPlatform::FallingPlatform(float x, float y, float w, float h)
    : Trap(x, y, w, h),
      m_falling(false), m_fallVelocity(0.0f),
      m_shakeTimer(0.0f), m_originX(x), m_originY(y)
{
    m_lethal = false;
}

void FallingPlatform::resetState() {
    // Restore to original position and un-fall
    m_x            = m_originX;
    m_y            = m_originY;
    m_falling      = false;
    m_fallVelocity = 0.0f;
    m_shakeTimer   = 0.0f;
}

void FallingPlatform::update(float dt) {
    if (m_shakeTimer > 0) {
        m_shakeTimer -= dt;
        // Do NOT move m_x here - that breaks collision detection.
        // Visual shake is handled in draw() using a draw offset.
    } else if (m_falling) {
        m_fallVelocity += GRAVITY * dt;
        m_y += m_fallVelocity * dt;
    }
}

void FallingPlatform::draw() const {
    // Visual shake offset (does not affect collision rect)
    float shakeOff = (m_shakeTimer > 0)
        ? (float)(GetRandomValue(-3, 3)) : 0.0f;

    Color c = (m_falling || m_shakeTimer > 0) ? RED : COL_FALLING_PLAT;
    Rectangle drawRect = { m_x + shakeOff, m_y, m_w, m_h };
    DrawRectangleRec(drawRect, c);
    DrawRectangleLinesEx(drawRect, 2, PURPLE);
    if (m_shakeTimer > 0)
        DrawText("!", (int)(m_x + m_w/2 - 4), (int)(m_y - 20), 18, RED);
}

void FallingPlatform::checkPlayerInteraction(Player& /*player*/) {}

// ============================================================
//  DisappearingPlatform
// ============================================================
DisappearingPlatform::DisappearingPlatform(float x, float y, float w, float h,
                                            float onTime, float offTime, float phase)
    : Trap(x, y, w, h),
      m_onTime(onTime), m_offTime(offTime),
      m_solid(true), m_cycleTimer(phase)
{}

void DisappearingPlatform::update(float dt) {
    m_cycleTimer += dt;
    if (m_solid) {
        if (m_cycleTimer >= m_onTime) { m_solid = false; m_cycleTimer = 0; }
    } else {
        if (m_cycleTimer >= m_offTime) { m_solid = true;  m_cycleTimer = 0; }
    }
}

void DisappearingPlatform::draw() const {
    if (!m_solid) {
        unsigned char alpha = (unsigned char)(60 + 40 * sinf((float)GetTime() * 6.0f));
        DrawRectangleRec(getRect(), { 137, 180, 130, alpha });
        return;
    }
    DrawRectangleRec(getRect(), COL_PLATFORM);
    DrawRectangleLinesEx(getRect(), 2, DARKGREEN);
}

void DisappearingPlatform::checkPlayerInteraction(Player& /*player*/) {}

// ============================================================
//  ReverseTrap
// ============================================================
ReverseTrap::ReverseTrap(float x, float y, float w, float h, float duration)
    : Trap(x, y, w, h),
      m_duration(duration), m_activeTimer(0), m_triggered(false)
{
    m_lethal = false;
}

void ReverseTrap::resetState() {
    m_triggered   = false;
    m_activeTimer = 0;
}

void ReverseTrap::update(float dt) {
    if (m_triggered && m_activeTimer > 0)
        m_activeTimer -= dt;
}

void ReverseTrap::draw() const {
    float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 3.0f);
    Color c = {
        (unsigned char)(180 * pulse), 40,
        (unsigned char)(180 * (1.0f - pulse)), 55
    };
    DrawRectangleRec(getRect(), c);
    int tw = MeasureText("?", 20);
    DrawText("?", (int)(m_x + m_w/2 - tw/2), (int)(m_y + m_h/2 - 10), 20, PURPLE);
}

void ReverseTrap::checkPlayerInteraction(Player& player) {
    if (CheckCollisionRecs(getRect(), player.getRect())) {
        if (!m_triggered) {
            m_triggered   = true;
            m_activeTimer = m_duration;
            player.setReverseControls(true);
        } else if (m_activeTimer <= 0) {
            player.setReverseControls(false);
        }
    } else {
        if (m_triggered && m_activeTimer <= 0) {
            player.setReverseControls(false);
            m_triggered = false;
        }
    }
}

// ============================================================
//  GoalDoor
// ============================================================
GoalDoor::GoalDoor(float x, float y)
    : Entity(x, y, TILE_SIZE, TILE_SIZE * 2), m_animTimer(0)
{}

void GoalDoor::update(float dt) { m_animTimer += dt; }

void GoalDoor::draw() const {
    float glow = 0.7f + 0.3f * sinf(m_animTimer * 3.0f);
    Color doorColor = {
        (unsigned char)(COL_GOAL.r * glow),
        (unsigned char)(COL_GOAL.g * glow),
        (unsigned char)(COL_GOAL.b * glow), 255
    };
    DrawRectangleRec(getRect(), doorColor);
    DrawRectangleLinesEx(getRect(), 3, DARKGREEN);
    DrawCircle((int)(m_x + m_w * 0.75f), (int)(m_y + m_h * 0.55f), 5, GOLD);
    DrawText("EXIT", (int)(m_x - 2), (int)(m_y - 22), 16, COL_GOAL);
    for (int i = 0; i < 3; ++i) {
        float angle = m_animTimer * 2.0f + i * 2.094f;
        DrawText("*",
            (int)(m_x + m_w/2 + cosf(angle) * 28),
            (int)(m_y + m_h/2 + sinf(angle) * 28),
            14, GOLD);
    }
}

void GoalDoor::checkPlayerInteraction(Player& player) {
    if (CheckCollisionRecs(getRect(), player.getRect()))
        player.setWon(true);
}