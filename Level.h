#pragma once
// ============================================================
//  Level.h  -  Manages one playable level
//
//  OOP: owns polymorphic Trap vector, uses STL stack/queue
// ============================================================
#include "Entity.h"
#include "Player.h"
#include "Trap.h"
#include "Constants.h"

#include <vector>
#include <memory>
#include <stack>
#include <queue>
#include <string>
#include <functional>

// Static platform tile
struct Platform {
    Rectangle rect;
    Color     color;
    bool      visible = true;
};

// Timed trap event: fires a lambda after a delay
struct TrapEvent {
    float                 delay;
    std::function<void()> action;
    bool                  fired;

    bool operator>(const TrapEvent& o) const { return delay > o.delay; }
};

class Level {
public:
    explicit Level(int levelNumber);
    ~Level() = default;

    void update(float dt);
    void draw()   const;
    void reset();

    bool    isComplete()  const { return m_complete; }
    bool    playerDead()  const;
    int     getLevelNum() const { return m_levelNumber; }
    Player& getPlayer()         { return *m_player; }
    Color   getBgColor()  const { return m_bgColor; }

private:
    void buildLevel1();
    void buildLevel2();
    void buildLevel3();
    void buildLevel4();

    void addPlatform(float x, float y, float w, float h,
                     Color c = COL_PLATFORM);

    void resolvePlayerCollisions();

    int                        m_levelNumber;
    std::unique_ptr<Player>    m_player;
    std::unique_ptr<GoalDoor>  m_goal;
    std::vector<Platform>      m_platforms;

    // Polymorphic container - stores any Trap subclass
    std::vector<std::unique_ptr<Trap>> m_traps;

    // Weak pointers into m_traps for fast per-type access
    std::vector<MovingPlatform*>   m_movingPlatforms;
    std::vector<FallingPlatform*>  m_fallingPlatforms;

    // STL stack: respawn checkpoint history
    std::stack<Vector2> m_checkpoints;

    // STL priority queue: timed trap events
    std::priority_queue<TrapEvent,
                        std::vector<TrapEvent>,
                        std::greater<TrapEvent>> m_eventQueue;

    float m_elapsed;
    bool  m_complete;
    Color m_bgColor;
};
