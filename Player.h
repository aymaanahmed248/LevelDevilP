#pragma once
// ============================================================
//  Player.h  -  The character the user controls
//
//  OOP: Inherits Entity, encapsulation with getters/setters,
//       copy constructor, friend class Level
// ============================================================
#include "Entity.h"
#include "Constants.h"
#include <vector>   // FIX: needed for std::vector in resolveCollisions

class Level;  // forward declaration for friend

class Player : public Entity {
public:
    Player(float spawnX, float spawnY);
    ~Player() override = default;

    // Copy constructor (required by OOP spec)
    Player(const Player& other);

    // Entity interface
    void update(float dt) override;
    void draw()    const  override;

    // Physics
    void handleInput(float dt);
    void applyGravity(float dt);
    void resolveCollisions(const std::vector<Rectangle>& platforms);

    // State getters
    bool  isDead()              const { return m_dead; }
    bool  hasWon()              const { return m_won; }
    bool  isOnGround()          const { return m_onGround; }
    bool  isReverseControls()   const { return m_reverseControls; }  // FIX: public getter
    float getVX()               const { return m_vx; }
    float getVY()               const { return m_vy; }
    int   getDeaths()           const { return m_deaths; }

    // State setters
    void setDead(bool v)              { m_dead = v; }
    void setWon(bool v)               { m_won  = v; }
    void setReverseControls(bool v)   { m_reverseControls = v; }
    void addDeath()                   { ++m_deaths; }

    // Respawn to spawn point
    void respawn();

    // friend: Level can read private members directly (OOP friend class demo)
    friend class Level;

private:
    float m_vx, m_vy;
    float m_spawnX, m_spawnY;
    bool  m_onGround;
    bool  m_dead;
    bool  m_won;
    bool  m_reverseControls;
    int   m_deaths;
    float m_deathFlashTimer;
};
