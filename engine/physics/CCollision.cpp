#include "CCollision.h"
#include <algorithm>
#include <cmath>
#include "../../game/entities/player/CMario.h"
#include "../../game/entities/blocks/CBridge.h"

bool CCollision::CheckAABB(const Box& a, const Box& b) {
    return !(a.right <= b.left || a.left >= b.right || a.top <= b.bottom || a.bottom >= b.top);
}

void CCollision::ResolveCollision(CGameObject* obj, float dt, const std::vector<CGameObject*>& coObjects) {
    if (!obj) return;

    // --- STEP 1: X-Axis Movement and Resolution ---
    obj->x += obj->vx * dt;

    float left1, bottom1, right1, top1;
    obj->GetBoundingBox(left1, bottom1, right1, top1);
    Box A = ToBox(left1, bottom1, right1, top1);
    float objWidth = right1 - left1;

    for (auto other : coObjects) {
        if (!other || other == obj) continue;

        // One-way platforms: no X collision at all
        CBridge* bridge = dynamic_cast<CBridge*>(other);
        if (bridge && bridge->isOneWay) continue;

        float left2, bottom2, right2, top2;
        other->GetBoundingBox(left2, bottom2, right2, top2);
        Box B = ToBox(left2, bottom2, right2, top2);

        if (CheckAABB(A, B)) {
            if (!other->IsSolid()) {
                other->OnCollision(obj);
                continue;
            }

            if (obj->vx > 0) {
                // Moving right, hit left side of 'other'
                obj->x = B.left - objWidth;
            } else if (obj->vx < 0) {
                // Moving left, hit right side of 'other'
                obj->x = B.right;
            }
            obj->vx = 0;

            // Re-update bounding box A for subsequent overlap checks in X
            obj->GetBoundingBox(left1, bottom1, right1, top1);
            A = ToBox(left1, bottom1, right1, top1);
        }
    }

    // --- STEP 2: Y-Axis Movement and Resolution ---
    float prevBottom = bottom1; // Bottom edge BEFORE Y movement
    obj->y += obj->vy * dt;

    obj->GetBoundingBox(left1, bottom1, right1, top1);
    A = ToBox(left1, bottom1, right1, top1);
    float objHeight = top1 - bottom1;

    for (auto other : coObjects) {
        if (!other || other == obj) continue;
        float left2, bottom2, right2, top2;
        other->GetBoundingBox(left2, bottom2, right2, top2);
        Box B = ToBox(left2, bottom2, right2, top2);

        // One-way platform: only block when falling onto it from above
        CBridge* bridge = dynamic_cast<CBridge*>(other);
        if (bridge && bridge->isOneWay) {
            // Skip if moving up (jumping through)
            if (obj->vy >= 0) continue;
            // Skip if bottom was already below bridge top before moving
            if (prevBottom < B.top) continue; // Was already overlapping or below bridge top
        }

        if (CheckAABB(A, B)) {
            if (!other->IsSolid()) {
                other->OnCollision(obj);
                continue;
            }

            if (obj->vy < 0) {
                // Falling down, hit top side of 'other'
                obj->y = B.top + 0.1f;
                obj->vy = 0;
                if (auto m = dynamic_cast<CMario*>(obj)) m->SetOnGround(true);
            } else if (obj->vy > 0) {
                // Jumping up, hit bottom side of 'other' (Ceiling)
                // <--- THIS is where Mario hits a block from below
                if (bridge && bridge->isOneWay) continue;
                obj->y = B.bottom - objHeight;
                obj->vy = 0;
                other->OnHitFromBelow(obj);
            }

            // Re-update bounding box A for subsequent overlap checks in Y
            obj->GetBoundingBox(left1, bottom1, right1, top1);
            A = ToBox(left1, bottom1, right1, top1);
        }
    }
}
