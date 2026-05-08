#pragma once
// ============================================================
//  Entity.h  -  Abstract base class for every game object
//
//  OOP: Abstract class (pure virtual update/draw),
//       Encapsulation (private id, protected position),
//       Operator overloading (== by id)
// ============================================================
#include "raylib.h"

class Entity {
public:
    explicit Entity(float x, float y, float w, float h);
    virtual ~Entity() = default;

    // Pure virtual - makes this class abstract
    virtual void update(float dt) = 0;
    virtual void draw()    const  = 0;

    // Getters (Encapsulation)
    Rectangle getRect()  const { return Rectangle{ m_x, m_y, m_w, m_h }; }
    float     getX()     const { return m_x; }
    float     getY()     const { return m_y; }
    float     getW()     const { return m_w; }
    float     getH()     const { return m_h; }
    bool      isActive() const { return m_active; }
    int       getId()    const { return m_id; }

    // Setters
    void setPosition(float x, float y) { m_x = x; m_y = y; }
    void setActive(bool v)             { m_active = v; }

    // Operator overloading: entities are equal if they share the same id
    bool operator==(const Entity& other) const { return m_id == other.m_id; }
    bool operator!=(const Entity& other) const { return !(*this == other); }

protected:
    float m_x, m_y;  // top-left position
    float m_w, m_h;  // width and height
    bool  m_active;

private:
    int        m_id;
    static int s_nextId;
};
