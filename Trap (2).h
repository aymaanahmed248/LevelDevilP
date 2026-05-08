#pragma once
// ============================================================
//  Trap.h  -  All trap types
//  Entity -> Trap -> SpikeTrap / MovingPlatform / FallingPlatform
//                    DisappearingPlatform / ReverseTrap
// ============================================================
#include "Entity.h"
#include "Constants.h"

class Player;

// ------------------------------------------------------------------
//  Base Trap
// ------------------------------------------------------------------
class Trap : public Entity {
public:
    Trap(float x, float y, float w, float h);
    virtual ~Trap() = default;

    virtual void checkPlayerInteraction(Player& player) = 0;

    // Called by Level::reset() so traps go back to initial state
    virtual void resetState() {}

    bool isLethal() const { return m_lethal; }

protected:
    bool  m_lethal;
    float m_timer;
};

// ------------------------------------------------------------------
//  SpikeTrap - hidden for 'delay' seconds, then pops up and kills
// ------------------------------------------------------------------
class SpikeTrap : public Trap {
public:
    SpikeTrap(float x, float y, float w, float h, float delay = 0.0f);
    ~SpikeTrap() override = default;

    void update(float dt)  override;
    void draw()    const   override;
    void checkPlayerInteraction(Player& player) override;
    void resetState()      override;   // hides spikes and resets timer

private:
    float m_delay;
    float m_initialDelay;   // kept so resetState can restore it
    bool  m_visible;
};

// ------------------------------------------------------------------
//  MovingPlatform
// ------------------------------------------------------------------
class MovingPlatform : public Trap {
public:
    MovingPlatform(float x, float y, float w, float h,
                   float speed, float range, int axis = 0);
    ~MovingPlatform() override = default;

    void update(float dt)  override;
    void draw()    const   override;
    void checkPlayerInteraction(Player& player) override;
    void carryPlayer(Player& player);

private:
    float m_speed;
    float m_range;
    int   m_axis;
    float m_origin;
    int   m_dir;
};

// ------------------------------------------------------------------
//  FallingPlatform
// ------------------------------------------------------------------
class FallingPlatform : public Trap {
public:
    FallingPlatform(float x, float y, float w, float h);
    ~FallingPlatform() override = default;

    void update(float dt)  override;
    void draw()    const   override;
    void checkPlayerInteraction(Player& player) override;
    void resetState()      override;   // resets to original position

    void triggerFall() { if (!m_falling) { m_falling = true; m_shakeTimer = 1.2f; } }
    bool isFalling()   const { return m_falling; }

private:
    bool  m_falling;
    float m_fallVelocity;
    float m_shakeTimer;
    float m_originX;
    float m_originY;   // stored so resetState can restore Y
};

// ------------------------------------------------------------------
//  DisappearingPlatform
// ------------------------------------------------------------------
class DisappearingPlatform : public Trap {
public:
    DisappearingPlatform(float x, float y, float w, float h,
                         float onTime, float offTime, float phase = 0.0f);
    ~DisappearingPlatform() override = default;

    void update(float dt)  override;
    void draw()    const   override;
    void checkPlayerInteraction(Player& player) override;

    bool isSolid() const { return m_solid; }

private:
    float m_onTime, m_offTime;
    bool  m_solid;
    float m_cycleTimer;
};

// ------------------------------------------------------------------
//  ReverseTrap
// ------------------------------------------------------------------
class ReverseTrap : public Trap {
public:
    ReverseTrap(float x, float y, float w, float h, float duration = 4.0f);
    ~ReverseTrap() override = default;

    void update(float dt)  override;
    void draw()    const   override;
    void checkPlayerInteraction(Player& player) override;
    void resetState()      override;

private:
    float m_duration;
    float m_activeTimer;
    bool  m_triggered;
};

// ------------------------------------------------------------------
//  GoalDoor
// ------------------------------------------------------------------
class GoalDoor : public Entity {
public:
    GoalDoor(float x, float y);
    ~GoalDoor() override = default;

    void update(float dt) override;
    void draw()    const  override;
    void checkPlayerInteraction(Player& player);

private:
    float m_animTimer;
};
