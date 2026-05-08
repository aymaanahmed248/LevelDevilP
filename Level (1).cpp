// ============================================================
//  Level.cpp
//
//  Y axis: 0 = top of screen, 720 = bottom
//  Ground top = Y=600.  Player feet at Y=600 means standing on ground.
//  Player spawn Y = 540 (60px above ground top, player H=48 so feet at 588).
//  Safe jump rise = 80px max per step.
// ============================================================
#include "Level.h"
#include "AudioManager.h"
#include <cmath>
#include <stdexcept>

Level::Level(int levelNumber)
    : m_levelNumber(levelNumber),
      m_elapsed(0), m_complete(false),
      m_bgColor(COL_BG_1)
{
    if (levelNumber < 1 || levelNumber > 4)
        throw std::out_of_range("Level number must be 1-4");

    switch (levelNumber) {
        case 1: buildLevel1(); break;
        case 2: buildLevel2(); break;
        case 3: buildLevel3(); break;
        case 4: buildLevel4(); break;
    }
}

void Level::addPlatform(float x, float y, float w, float h, Color c) {
    m_platforms.push_back({ {x, y, w, h}, c, true });
}

// ============================================================
//  LEVEL 1 - Tutorial
//  Gentle staircase, 80px rise each step, one disappearing platform.
// ============================================================
void Level::buildLevel1() {
    m_bgColor = {24, 24, 37, 255};

    m_player = std::make_unique<Player>(80, 540);

    // Ground
    addPlatform(0, 600, 1280, 120);

    // Step 1  Y=520 (+80)
    addPlatform(200, 520, 180, 20);

    // Step 2  Y=440 (+80)
    addPlatform(430, 440, 180, 20);

    // Step 3  Y=360 (+80) solid - safe reference
    addPlatform(640, 360, 180, 20);

    // TROLL: looks same as step 3, but disappears after 2.5s
    auto dp = std::make_unique<DisappearingPlatform>(860, 360, 180, 20,
                                                      2.5f, 1.5f, 0.0f);
    m_traps.push_back(std::move(dp));

    // Safe landing
    addPlatform(1060, 360, 180, 20);

    // Goal on the last platform
    m_goal = std::make_unique<GoalDoor>(1130, 264);

    m_checkpoints.push({80, 540});
}

// ============================================================
//  LEVEL 2 - Medium
//  Moving platform over pit, delayed spikes, disappearing near goal.
//  All platforms reachable with 80px max step rise.
// ============================================================
void Level::buildLevel2() {
    m_bgColor = {20, 18, 35, 255};

    m_player = std::make_unique<Player>(60, 540);

    // Left ground
    addPlatform(0, 600, 500, 120);

    // Step 1  Y=520 (+80)
    addPlatform(160, 520, 180, 20);

    // Step 2  Y=520 same height (walk right, has delayed spikes)
    addPlatform(380, 520, 180, 20);
    auto spike1 = std::make_unique<SpikeTrap>(380, 500, 180, 20, 2.0f);
    m_traps.push_back(std::move(spike1));

    // Moving platform crosses the pit  (x=500 to x=700) at Y=520
    auto mp = std::make_unique<MovingPlatform>(500, 520, 160, 20,
                                               75.0f, 180.0f, 0);
    m_movingPlatforms.push_back(mp.get());
    m_traps.push_back(std::move(mp));

    // Right ground
    addPlatform(700, 600, 580, 120);

    // Step 3  Y=520 (+80 from right ground)
    addPlatform(760, 520, 180, 20);

    // Step 4  Y=440 (+80)
    addPlatform(960, 440, 180, 20);
    auto spike2 = std::make_unique<SpikeTrap>(960, 420, 180, 20, 3.5f);
    m_traps.push_back(std::move(spike2));

    // Step 5  Y=360 (+80) - disappears near goal
    addPlatform(1100, 360, 170, 20);
    auto dp = std::make_unique<DisappearingPlatform>(1100, 360, 170, 20,
                                                      1.8f, 1.1f, 0.0f);
    m_traps.push_back(std::move(dp));

    // Goal platform Y=280 (+80)
    addPlatform(1110, 280, 170, 20);
    m_goal = std::make_unique<GoalDoor>(1150, 184);

    m_checkpoints.push({60, 540});
}

// ============================================================
//  LEVEL 3 - Hard
//
//  A straight left-to-right path at GROUND LEVEL.
//  The entire level is one continuous floor with sections.
//  No floating platforms - everything is connected.
//
//  Layout (all at ground Y=600, player walks at Y=540):
//
//  [Start]---[Ground A]---[FallingPlat x4]---[Ground B]---
//  ---[Walkway step up]---[Reverse zone]---[Spike zone]---
//  ---[Step up]---[Step up]---[MovingPlat]---[Goal]
// ============================================================
void Level::buildLevel3() {
    // --------------------------------------------------------
    // LEVEL 3 - Hard  (CLEAN REDESIGN)
    //
    // Simple flat left-to-right level. NO floating platforms.
    // The entire path is at ONE height (ground Y=580).
    // Falling platforms are just normal ground sections that
    // drop when stepped on. Gaps between ground sections force
    // the player to jump. Troll mechanics come from spikes +
    // reverse controls on the right side.
    //
    // X layout:
    //  [Ground  x=0..300   Y=580]   <- safe start
    //  [FallPlat x=300..520  Y=580] <- falls when stepped on
    //  [Gap      x=520..580]        <- pit (40px, easy hop)
    //  [FallPlat x=580..800  Y=580] <- falls when stepped on
    //  [Ground  x=800..1280  Y=580] <- safe right side
    //  [Walkway x=800..1260  Y=500] <- 80px step up on right
    //  [Reverse zone on walkway]
    //  [Spikes on walkway after 4s]
    //  [Steps up to goal]
    // --------------------------------------------------------
    m_bgColor = {18, 14, 28, 255};

    // Player spawns on safe left ground. Y=515 so feet=563, above ground top 580? 
    // Ground top is Y=580, player H=48, so spawn Y = 580-48 = 532.
    m_player = std::make_unique<Player>(60, 532);

    // Safe starting ground (solid, no tricks)
    addPlatform(0,   580, 300, 140);

    // Falling platform 1 - looks identical to ground, falls when stepped on
    {
        auto fp = std::make_unique<FallingPlatform>(300, 580, 220, 140);
        m_fallingPlatforms.push_back(fp.get());
        m_traps.push_back(std::move(fp));
    }

    // Small solid bridge in the middle so level is crossable
    addPlatform(520, 580, 60, 140);

    // Falling platform 2
    {
        auto fp = std::make_unique<FallingPlatform>(580, 580, 220, 140);
        m_fallingPlatforms.push_back(fp.get());
        m_traps.push_back(std::move(fp));
    }

    // Safe right ground
    addPlatform(800, 580, 480, 140);

    // Step up 80px to walkway (Y=500)
    addPlatform(810, 500, 440, 20);

    // Reverse controls zone sits on the walkway
    auto rev = std::make_unique<ReverseTrap>(820, 420, 420, 80, 5.0f);
    m_traps.push_back(std::move(rev));

    // Spikes appear on walkway after 4s
    auto spike = std::make_unique<SpikeTrap>(810, 480, 440, 20, 4.0f);
    m_traps.push_back(std::move(spike));

    // Climb: step up 80px (Y=420)
    addPlatform(1060, 420, 200, 20);

    // Climb: step up 80px (Y=340)
    addPlatform(1080, 340, 180, 20);

    // Moving platform (vertical, near goal)
    auto mp = std::make_unique<MovingPlatform>(1010, 275, 130, 20,
                                               50.0f, 60.0f, 1);
    m_movingPlatforms.push_back(mp.get());
    m_traps.push_back(std::move(mp));

    // Goal platform + door
    addPlatform(1090, 260, 180, 20);
    m_goal = std::make_unique<GoalDoor>(1150, 164);

    m_checkpoints.push({60, 532});
}


// ============================================================
//  LEVEL 4 - Final: all mechanics combined
//
//  Path (left-to-right at ground then climb):
//  Start ledge -> 5 disappearing platforms (small hop) ->
//  island (hidden spikes) -> climb steps ->
//  reverse zone -> falling platforms -> moving platform -> goal
// ============================================================
void Level::buildLevel4() {
    m_bgColor = {12, 10, 22, 255};

    m_player = std::make_unique<Player>(50, 540);

    // Start ledge
    addPlatform(0, 600, 150, 120);

    // 5 disappearing platforms, small 40px hop from ground
    for (int i = 0; i < 5; ++i) {
        float phase = i * 0.4f;
        float offT  = (i % 2 == 0) ? 0.7f : 1.2f;
        auto dp = std::make_unique<DisappearingPlatform>(
            130.0f + i * 190.0f, 560, 150, 20,
            1.1f, offT, phase);
        m_traps.push_back(std::move(dp));
    }

    // Island with hidden spikes (appears 1.5s in)
    addPlatform(1080, 600, 200, 120);
    auto spike1 = std::make_unique<SpikeTrap>(1085, 580, 190, 20, 1.5f);
    m_traps.push_back(std::move(spike1));

    // Climb: step A +80 (Y=520)
    addPlatform(1060, 520, 190, 20);

    // Climb: step B +80 (Y=440)
    addPlatform(940, 440, 180, 20);

    // Climb: step C +80 (Y=360)
    addPlatform(820, 360, 180, 20);

    // Reverse zone above step C
    auto rev = std::make_unique<ReverseTrap>(830, 280, 160, 80, 6.0f);
    m_traps.push_back(std::move(rev));

    // Falling platforms (Y=360, going left)
    {
        auto fp1 = std::make_unique<FallingPlatform>(660, 360, 140, 20);
        m_fallingPlatforms.push_back(fp1.get());
        m_traps.push_back(std::move(fp1));

        auto fp2 = std::make_unique<FallingPlatform>(490, 280, 140, 20);
        m_fallingPlatforms.push_back(fp2.get());
        m_traps.push_back(std::move(fp2));
    }

    // Moving platform (Y=240)
    auto mp = std::make_unique<MovingPlatform>(310, 240, 140, 20,
                                               110.0f, 160.0f, 0);
    m_movingPlatforms.push_back(mp.get());
    m_traps.push_back(std::move(mp));

    // Final approach + timed spike
    addPlatform(180, 180, 170, 20);
    auto spike2 = std::make_unique<SpikeTrap>(180, 160, 170, 20, 6.0f);
    m_traps.push_back(std::move(spike2));

    // Last step and goal
    addPlatform(60, 100, 160, 20);
    m_goal = std::make_unique<GoalDoor>(90, 4);

    m_checkpoints.push({50, 540});
}

// ============================================================
//  reset() - respawn and reset ALL trap states
// ============================================================
void Level::reset() {
    m_elapsed  = 0;
    m_complete = false;

    // Restore checkpoint stack
    std::stack<Vector2> temp;
    while (!m_checkpoints.empty()) {
        temp.push(m_checkpoints.top());
        m_checkpoints.pop();
    }
    while (!temp.empty()) {
        m_checkpoints.push(temp.top());
        temp.pop();
    }

    m_player->respawn();

    // Call resetState() on every trap so spikes/falling/reverse all reset
    for (auto& t : m_traps) {
        t->setActive(true);
        t->resetState();   // <-- KEY FIX: resets timer, visibility, position
    }
}

// ============================================================
//  resolvePlayerCollisions()
// ============================================================
void Level::resolvePlayerCollisions() {
    std::vector<Rectangle> solids;

    for (const auto& p : m_platforms)
        if (p.visible) solids.push_back(p.rect);

    for (auto* mp : m_movingPlatforms)
        if (mp->isActive()) solids.push_back(mp->getRect());

    for (auto& t : m_traps) {
        auto* dp = dynamic_cast<DisappearingPlatform*>(t.get());
        if (dp && dp->isActive() && dp->isSolid())
            solids.push_back(dp->getRect());
    }

    for (auto* fp : m_fallingPlatforms)
        if (fp->isActive() && !fp->isFalling())
            solids.push_back(fp->getRect());

    bool wasOnGround = m_player->isOnGround();
    m_player->resolveCollisions(solids);

    if (!wasOnGround && m_player->isOnGround())
        AudioManager::getInstance().playSound("land");

    if (m_player->isOnGround()) {
        Rectangle feet = {
            m_player->getX(),
            m_player->getY() + m_player->getH() - 4,
            m_player->getW(), 8
        };
        for (auto* fp : m_fallingPlatforms) {
            if (fp->isActive() && !fp->isFalling())
                if (CheckCollisionRecs(feet, fp->getRect()))
                    fp->triggerFall();
        }
    }
}

// ============================================================
//  update()
// ============================================================
void Level::update(float dt) {
    if (m_complete) return;

    m_elapsed += dt;

    m_player->handleInput(dt);
    m_player->applyGravity(dt);
    resolvePlayerCollisions();
    m_player->update(dt);

    for (auto& t : m_traps) {
        if (!t->isActive()) continue;
        t->update(dt);
        auto* trap = dynamic_cast<Trap*>(t.get());
        if (trap) trap->checkPlayerInteraction(*m_player);
    }

    for (auto* mp : m_movingPlatforms) {
        if (!mp->isActive()) continue;
        Rectangle feet = {
            m_player->getX(),
            m_player->getY() + m_player->getH() - 6,
            m_player->getW(), 10
        };
        if (CheckCollisionRecs(feet, mp->getRect()) && m_player->isOnGround())
            mp->carryPlayer(*m_player);
    }

    if (m_goal) {
        m_goal->update(dt);
        m_goal->checkPlayerInteraction(*m_player);
        if (m_player->hasWon()) {
            m_complete = true;
            AudioManager::getInstance().playSound("win");
        }
    }

    if (m_player->isDead())
        m_player->addDeath();
}

// ============================================================
//  draw()
// ============================================================
void Level::draw() const {
    for (const auto& p : m_platforms) {
        if (!p.visible) continue;
        DrawRectangleRec(p.rect, p.color);
        DrawRectangleLinesEx(p.rect, 2, ColorBrightness(p.color, -0.3f));
    }

    for (const auto& t : m_traps) {
        if (!t->isActive()) continue;
        t->draw();
    }

    if (m_goal) m_goal->draw();
    m_player->draw();

    DrawText(TextFormat("Level %d", m_levelNumber),
             SCREEN_W - 110, SCREEN_H - 28, 18, DARKGRAY);
}

bool Level::playerDead() const {
    return m_player && m_player->isDead();
}
