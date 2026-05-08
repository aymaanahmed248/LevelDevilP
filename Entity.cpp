// ============================================================
//  Entity.cpp
// ============================================================
#include "Entity.h"

int Entity::s_nextId = 0;

Entity::Entity(float x, float y, float w, float h)
    : m_x(x), m_y(y), m_w(w), m_h(h),
      m_active(true),
      m_id(s_nextId++)
{}
