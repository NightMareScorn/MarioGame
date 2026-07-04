#pragma once
#include "../CGameObject.h"
#include <string>
#include <cmath>

// Decorative block - renders an animation but has NO collision.
// Used for background elements like clouds, flag pole, etc.
class CDecorBlock : public CGameObject {
    std::string aniName;
    bool isDrifting = false;  // For clouds: gentle horizontal drift
    float driftTime = 0.0f;
    float baseX = 0.0f;
public:
    CDecorBlock(float x, float y, std::string aniName);
    void SetDrifting(bool d) { isDrifting = d; baseX = x; }
    std::string GetAniName() const { return aniName; }
    virtual bool IsSolid() override { return false; }
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
};
