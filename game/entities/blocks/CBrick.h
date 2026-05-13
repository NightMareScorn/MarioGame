#pragma once
#include <string>
#include "CBlock.h"

class CBrick : public CBlock {
    std::string aniName;
public:
    CBrick(float x, float y, std::string aniName = "ANI_BRICK_IDLE") : CBlock(x, y), aniName(aniName) {}
    void Update(float dt) override {}
    void Render() override;
    void GetBoundingBox(float &l, float &t, float &r, float &b) override;
};
