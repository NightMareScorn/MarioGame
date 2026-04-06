#pragma once
#include <vector>
#include "../../game/entities/CGameObject.h"

class CCollision {
public:
    struct Box { float l,t,r,b; };

    static inline Box ToBox(float l,float t,float r,float b) { return Box{l,t,r,b}; }

    static bool CheckAABB(const Box &a, const Box &b);

    // Resolve collisions between `obj` and list of coObjects (static blocks etc.)
    static void ResolveCollision(CGameObject* obj, float dt, const std::vector<CGameObject*>& coObjects);
};
