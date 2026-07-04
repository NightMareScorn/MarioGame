#pragma once
#include "../CGameObject.h"
#include "../blocks/CBlock.h"

class CFireball : public CGameObject {
    float lifetimeMs;
    bool dead;
    int bounceCount;
public:
    CFireball(float x, float y, int direction);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
    bool IsDead() const { return dead; }
    void Kill() { dead = true; }

    bool IsSolid() override { return false; }
    bool IsBlocking(CGameObject *other) override {
        // Chỉ va chạm chặn với gạch/cống để nảy và nổ khi đâm vào tường
        return dynamic_cast<class CBlock *>(other) != nullptr;
    }
    void OnCollisionX(CGameObject *other, float nx) override;
    void OnCollisionY(CGameObject *other, float ny) override;
};
