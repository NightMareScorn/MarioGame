#pragma once
#include "../CGameObject.h"
#include <string>

// Decorative block - renders an animation but has NO collision.
// Used for background elements like clouds, flag pole, etc.
class CDecorBlock : public CGameObject {
    std::string aniName;
public:
    CDecorBlock(float x, float y, std::string aniName);
    void Update(float dt) override {}
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
};
