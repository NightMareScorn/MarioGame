#pragma once
#include "CBlock.h"
#include <string>

class CBrick : public CBlock {
    std::string aniName;
    bool isInvisible;
public:
    CBrick(float x, float y, std::string aniName = "ANI_BRICK_IDLE", bool isInvisible = false);
    void Update(float dt) override {}
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
    void OnHitFromBelow(CGameObject* hitter) override;

    bool IsInvisible() { return isInvisible; }
    void SetInvisible(bool value) { isInvisible = value; }
};
