#pragma once
#include <vector>
#include "../../game/entities/CGameObject.h"

class CCollision {
public:
    struct Box { float left, bottom, right, top; };

    static inline Box ToBox(float left, float bottom, float right, float top) { return Box{left, bottom, right, top}; }

    static bool CheckAABB(const Box &a, const Box &b);

    // Resolve collisions between `obj` and list of coObjects (static blocks etc.)
    static void ResolveCollision(CGameObject* obj, float dt, const std::vector<CGameObject*>& coObjects);
};
