#include "CCollision.h"
#include <algorithm>
#include <cmath>
#include "../../game/entities/player/CMario.h"

bool CCollision::CheckAABB(const Box &a, const Box &b) {
    return !(a.r <= b.l || a.l >= b.r || a.b <= b.t || a.t >= b.b);
}

void CCollision::ResolveCollision(CGameObject* obj, float dt, const std::vector<CGameObject*>& coObjects) {
    if (!obj) return;

    // --- STEP 1: X-Axis Movement and Resolution ---
    obj->x += obj->vx * dt;

    float l1, t1, r1, b1;
    obj->GetBoundingBox(l1, t1, r1, b1);
    Box A = ToBox(l1, t1, r1, b1);
    float objWidth = r1 - l1;

    for (auto other : coObjects) {
        if (!other || other == obj) continue;
        float l2, t2, r2, b2;
        other->GetBoundingBox(l2, t2, r2, b2);
        Box B = ToBox(l2, t2, r2, b2);

        if (CheckAABB(A, B)) {
            // Resolve X axis
            if (obj->vx > 0) {
                // Moving right, hit left side of 'other'
                obj->x = B.l - objWidth;
            } else if (obj->vx < 0) {
                // Moving left, hit right side of 'other'
                obj->x = B.r;
            }
            obj->vx = 0;
            
            // Re-update bounding box A for subsequent overlap checks in X
            obj->GetBoundingBox(l1, t1, r1, b1);
            A = ToBox(l1, t1, r1, b1);
        }
    }

    // --- STEP 2: Y-Axis Movement and Resolution ---
    obj->y += obj->vy * dt;

    obj->GetBoundingBox(l1, t1, r1, b1);
    A = ToBox(l1, t1, r1, b1);
    float objHeight = b1 - t1;

    for (auto other : coObjects) {
        if (!other || other == obj) continue;
        float l2, t2, r2, b2;
        other->GetBoundingBox(l2, t2, r2, b2);
        Box B = ToBox(l2, t2, r2, b2);

        if (CheckAABB(A, B)) {
            // Resolve Y axis
            if (obj->vy < 0) {
                // Falling down, hit top side of 'other'
                obj->y = B.b + 0.1f;
                obj->vy = 0;
                if (auto m = dynamic_cast<CMario*>(obj)) m->SetOnGround(true);
            } else if (obj->vy > 0) {
                // Jumping up, hit bottom side of 'other' (Ceiling) // <--- THIS is where Mario hits a block from below
                obj->y = B.t - objHeight;
                obj->vy = 0;
                other->OnHitFromBelow(obj);
            }
            
            // Re-update bounding box A for subsequent overlap checks in Y
            obj->GetBoundingBox(l1, t1, r1, b1);
            A = ToBox(l1, t1, r1, b1);
        }
    }
}
