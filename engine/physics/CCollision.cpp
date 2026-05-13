#include "CCollision.h"
#include <algorithm>
#include <cmath>
#include "../../game/entities/player/CMario.h"
#include "../../game/entities/blocks/CBridge.h"

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
        
        // One-way platforms: no X collision at all
        CBridge* bridge = dynamic_cast<CBridge*>(other);
        if (bridge && bridge->isOneWay) continue;
        
        float l2, t2, r2, b2;
        other->GetBoundingBox(l2, t2, r2, b2);
        Box B = ToBox(l2, t2, r2, b2);

        if (CheckAABB(A, B)) {
            if (!other->IsSolid()) {
                other->OnCollision(obj);
                continue;
            }

            if (obj->vx > 0) {
                obj->x = B.l - objWidth;
            } else if (obj->vx < 0) {
                obj->x = B.r;
            }
            obj->vx = 0;
            
            obj->GetBoundingBox(l1, t1, r1, b1);
            A = ToBox(l1, t1, r1, b1);
        }
    }

    // --- STEP 2: Y-Axis Movement and Resolution ---
    float prevBottom = b1; // Bottom edge BEFORE Y movement
    obj->y += obj->vy * dt;

    obj->GetBoundingBox(l1, t1, r1, b1);
    A = ToBox(l1, t1, r1, b1);
    float objHeight = b1 - t1;

    for (auto other : coObjects) {
        if (!other || other == obj) continue;
        float l2, t2, r2, b2;
        other->GetBoundingBox(l2, t2, r2, b2);
        Box B = ToBox(l2, t2, r2, b2);

        // One-way platform: only block when falling onto it from above
        CBridge* bridge = dynamic_cast<CBridge*>(other);
        if (bridge && bridge->isOneWay) {
            // Skip if moving up (jumping through)
            if (obj->vy >= 0) continue;
            // Skip if bottom was already below bridge top before moving
            if (prevBottom < B.b) continue; // Was already overlapping or below
        }

        if (CheckAABB(A, B)) {
            if (!other->IsSolid()) {
                other->OnCollision(obj);
                continue;
            }

            if (obj->vy < 0) {
                // Falling down, land on top
                obj->y = B.b + 0.1f;
                obj->vy = 0;
                if (auto m = dynamic_cast<CMario*>(obj)) m->SetOnGround(true);
            } else if (obj->vy > 0) {
                // Jumping up, hit ceiling (skip for one-way)
                if (bridge && bridge->isOneWay) continue;
                obj->y = B.t - objHeight;
                obj->vy = 0;
                other->OnHitFromBelow(obj);
            }
            
            obj->GetBoundingBox(l1, t1, r1, b1);
            A = ToBox(l1, t1, r1, b1);
        }
    }
}
