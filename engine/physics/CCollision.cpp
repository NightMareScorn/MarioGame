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

    for (auto other : coObjects) {
        if (!other || other == obj) continue;

        float l2,t2,r2,b2;
        other->GetBoundingBox(l2,t2,r2,b2);

        Box A = ToBox(l1,t1,r1,b1);
        Box B = ToBox(l2,t2,r2,b2);

        if (!CheckAABB(A,B)) continue;

        float overlapX = (std::min)(r1,r2) - (std::max)(l1,l2);
        float overlapY = (std::min)(b1,b2) - (std::max)(t1,t2);

        const float EPS_NUDGE = 0.5f;
        const float AXIS_EPS = 0.01f;

        bool preferY = false;
        // prefer Y resolution when object has vertical motion
        if (std::abs(obj->vy) > 0.0001f) preferY = true;

        if (!preferY) {
            // choose minimal axis
            preferY = (overlapY <= overlapX + AXIS_EPS);
        }

        if (!preferY) {
            // resolve X axis
            if (l1 < l2) {
                obj->x -= overlapX;
            } else {
                obj->x += overlapX;
            }
            obj->vx = 0;
        } else {
            // resolve Y axis
            float objHeight = b1 - t1;
            if (t1 > t2) {
                // obj hits from above -> land on top
                obj->y = b2;
                obj->vy = 0;
                if (auto m = dynamic_cast<CMario*>(obj)) m->SetOnGround(true);
            } else {
                // obj hits from below -> push down
                obj->y = t2 - objHeight;
                obj->vy = 0;
            }
        }

        // small nudge to avoid corner-sticking: if on ground and tiny horizontal overlap
        if (auto m = dynamic_cast<CMario*>(obj)) {
            float smallOverlapX = (std::min)(r1,r2) - (std::max)(l1,l2);
            if (m->IsOnGround() && smallOverlapX > 0 && smallOverlapX < 1.0f) {
                if (m->x < other->x) m->x -= EPS_NUDGE; else m->x += EPS_NUDGE;
            }
        }

        // update bounding box for subsequent checks (if moved)
        obj->GetBoundingBox(l1,t1,r1,b1);
    }
}
