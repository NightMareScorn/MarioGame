#pragma once
#include "CGoomba.h"
#include "../../registry/CResourceRegistry.h"

class CCheepCheep : public CGoomba {
    float limit = 32.0f; // Default +-2 tiles
public:
    CCheepCheep(float x, float y) : CGoomba(x, y) {
        spriteName = "ANI_CHEEP_CHEEP_RED";
        type = "cheepcheep";
        vx = -GOOMBA_WALKING_SPEED * 0.63f; // 0.63x speed (0.7 * 0.9)
        nx = 1; // Sprite defaults to facing left, so nx=1 means don't flip
    }
    void SetLimit(float l) { limit = l; }
    void Update(float dt) override;
    void Render() override {
        CResourceRegistry::GetInstance()->RenderAnimation(spriteName, x, y, nx);
    }
};
