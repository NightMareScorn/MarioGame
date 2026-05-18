#pragma once
#include "../CGameObject.h"

class CFireball : public CGameObject {
    float lifetimeMs;
    bool dead;
public:
    CFireball(float x, float y, int direction);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
    bool IsDead() const { return dead; }
    void Kill() { dead = true; }
};
