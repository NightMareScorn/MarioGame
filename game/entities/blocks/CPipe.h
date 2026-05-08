#pragma once
#include "CBlock.h"
#include <string>

// Pipe block - renders a visible green pipe and has solid collision.
// Uses assembled pipe sprites from tileset.png.
// Pipe dimensions: 32px wide (2 tiles), variable height.
class CPipe : public CBlock {
    float width;
    float height;
    std::string aniName;
    std::string destMap;
    std::string enterDirection;
public:
    CPipe(float x, float y, float width, float height, std::string aniName = "ANI_PIPE_UPWARDS_ASSEMBLED", std::string destMap = "", std::string enterDirection = "");
    void Update(float dt) override {}
    void Render() override;
    void GetBoundingBox(float &l, float &t, float &r, float &b) override;

    std::string GetDestMap() { return destMap; }
    std::string GetEnterDirection() { return enterDirection; }
    bool IsWarpPipe() { return !destMap.empty(); }
};
