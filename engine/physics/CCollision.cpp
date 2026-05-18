#include "CCollision.h"
#include <algorithm>
#include <cmath>
#include "../../game/entities/blocks/CBlock.h"
#include "../../game/entities/player/CMario.h"

bool CCollision::CheckAABB(const Box &a, const Box &b) {
    return !(a.right <= b.left || a.left >= b.right || a.top <= b.bottom || a.bottom >= b.top);
}

void CCollision::ResolveCollision(CGameObject* obj, float dt, const std::vector<CGameObject*>& coObjects) {
    // --- TRỤC X ---
    obj->x += obj->vx * dt;
    float l1, b1, r1, t1;
    obj->GetBoundingBox(l1, b1, r1, t1);

    for (auto other : coObjects) {
        if (!other || other == obj || other->IsDead()) continue;
        float l2, b2, r2, t2;
        other->GetBoundingBox(l2, b2, r2, t2);

        if (CheckAABB({l1, b1, r1, t1}, {l2, b2, r2, t2})) {
            CMario* mario = dynamic_cast<CMario*>(obj);
            if (mario && other->IsItem()) {
                other->OnCollected(mario);
            }
            if (dynamic_cast<CBlock*>(other)) {
                if (obj->vx > 0) obj->x = l2 - (r1 - l1);
                else if (obj->vx < 0) obj->x = r2;
                
                if (obj->IsEnemy()) { // Quái chạm tường -> Đảo chiều
                    obj->vx = -obj->vx;
                    obj->nx = -obj->nx;
                } else obj->vx = 0;
            }
            obj->GetBoundingBox(l1, b1, r1, t1);
        }
    }

    // --- TRỤC Y ---
    obj->y += obj->vy * dt;
    obj->GetBoundingBox(l1, b1, r1, t1);

    for (auto other : coObjects) {
        if (!other || other == obj || other->IsDead()) continue;
        float l2, b2, r2, t2;
        other->GetBoundingBox(l2, b2, r2, t2);

        if (CheckAABB({l1, b1, r1, t1}, {l2, b2, r2, t2})) {
            CMario* mario = dynamic_cast<CMario*>(obj);

            if (mario && other->IsItem()) {
                other->OnCollected(mario);
            }
            else if (mario && other->IsEnemy()) {
                // STOMP
                if (mario->vy < 0 && b1 > b2) {
                    other->OnStomped();
                    mario->StompBounce();
                } else if (!mario->IsInvincible()) mario->Hurt();
            }
            else if (dynamic_cast<CBlock*>(other)) {
                if (obj->vy < 0) {
                    obj->y = t2; obj->vy = 0;
                    if (mario) mario->SetOnGround(true);
                } else if (obj->vy > 0) {
                    obj->y = b2 - (t1 - b1); obj->vy = 0;
                    other->OnHitFromBelow(obj);
                }
            }
            obj->GetBoundingBox(l1, b1, r1, t1);
        }
    }
}
