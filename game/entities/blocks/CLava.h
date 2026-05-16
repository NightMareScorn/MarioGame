#pragma once
#include "CBlock.h"
#include "../../entities/player/CMario.h"

class CLava : public CBlock {
    std::string aniName;
public:
    CLava(float x, float y, std::string aniName) : CBlock(x, y), aniName(aniName) {}
    virtual void Update(float dt) override {}
    virtual bool IsSolid() override { return false; }
    virtual void OnCollision(CGameObject* obj) override {
        if (auto mario = dynamic_cast<CMario*>(obj)) {
            mario->StartDeath();
        }
    }
    virtual void Render() override {
        CResourceRegistry::GetInstance()->RenderAnimation(aniName, x, y);
    }
    virtual void GetBoundingBox(float &l, float &t, float &r, float &b) override {
        l = x; t = y; r = x + 16; b = y + 16;
    }
};
