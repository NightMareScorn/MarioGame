#include "CCollision.h"
#include <algorithm>
#include <cmath>
#include "../../game/entities/player/CMario.h"

bool CCollision::CheckAABB(const Box &a, const Box &b) {
    return !(a.r <= b.l || a.l >= b.r || a.b <= b.t || a.t >= b.b);
}

void CCollision::ResolveCollision(CGameObject* obj, const std::vector<CGameObject*>& coObjects) {
    if (!obj) return;

    float l1,t1,r1,b1;
    obj->GetBoundingBox(l1,t1,r1,b1);

    struct CollisionEvent {
        CGameObject* other;
        float overlapArea;
    };
    std::vector<CollisionEvent> events;

    for (auto other : coObjects) {
        if (!other || other == obj) continue;
        float l2,t2,r2,b2;
        other->GetBoundingBox(l2,t2,r2,b2);

        Box A = ToBox(l1,t1,r1,b1);
        Box B = ToBox(l2,t2,r2,b2);

        if (CheckAABB(A,B)) {
            float overlapX = (std::min)(A.r, B.r) - (std::max)(A.l, B.l);
            float overlapY = (std::min)(A.b, B.b) - (std::max)(A.t, B.t);
            events.push_back({other, overlapX * overlapY});
        }
    }

    // Sort by largest overlap area to fix ghost collisions completely
    std::sort(events.begin(), events.end(), [](const CollisionEvent& a, const CollisionEvent& b) {
        return a.overlapArea > b.overlapArea;
    });

    for (auto ev : events) {
        auto other = ev.other;

        // Re-get bounding box because obj might have moved
        obj->GetBoundingBox(l1,t1,r1,b1);
        float l2,t2,r2,b2;
        other->GetBoundingBox(l2,t2,r2,b2);

        Box A = ToBox(l1,t1,r1,b1);
        Box B = ToBox(l2,t2,r2,b2);

        if (!CheckAABB(A,B)) continue;

        float overlapX = (std::min)(A.r, B.r) - (std::max)(A.l, B.l);
        float overlapY = (std::min)(A.b, B.b) - (std::max)(A.t, B.t);

        float center1_x = (A.l + A.r) * 0.5f;
        float center2_x = (B.l + B.r) * 0.5f;
        float center1_y = (A.t + A.b) * 0.5f;
        float center2_y = (B.t + B.b) * 0.5f;

        // Choose minimal axis to resolve (shortest penetration distance)
        bool preferY = (overlapY <= overlapX);

        if (!preferY) {
            // resolve X axis precisely away from center
            if (center1_x < center2_x) {
                obj->x -= overlapX;
            } else {
                obj->x += overlapX;
            }
            obj->vx = 0;
        } else {
            // resolve Y axis precisely away from center
            float objHeight = A.b - A.t;
            if (center1_y > center2_y) {
                // obj center is higher -> hits from above -> land on top
                obj->y = B.b;
                obj->vy = 0;
                if (auto m = dynamic_cast<CMario*>(obj)) m->SetOnGround(true);
            } else {
                // obj center is lower -> hits from below -> bonk head / push down
                obj->y = B.t - objHeight;
                obj->vy = 0;
            }
        }

        // update bounding box for subsequent checks (if moved)
        obj->GetBoundingBox(l1,t1,r1,b1);
    }
}
