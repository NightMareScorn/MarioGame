#pragma once
#include "CGoomba.h"
#include "../../registry/CResourceRegistry.h"

class CCheepCheep : public CGoomba {
    float limit = 32.0f; // Default +-2 tiles
public:
    CCheepCheep(float x, float y) : CGoomba(x, y) {
        spriteName = "ANI_CHEEP_CHEEP_RED";
        type = "cheepcheep";
    }
    void SetLimit(float l) { limit = l; }
    void Update(float dt) override;
    void Render() override {
        CResourceRegistry::GetInstance()->RenderAnimation(spriteName, x, y, nx);
    }
};
