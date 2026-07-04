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
    std::string name;
    std::string destPipe;
public:
    CPipe(float x, float y, float width, float height, std::string aniName = "ANI_PIPE_UPWARDS_ASSEMBLED", std::string destMap = "", std::string enterDirection = "");
    void Update(float dt) override {}
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;

    std::string GetDestMap() { return destMap; }
    std::string GetEnterDirection() { return enterDirection; }
    bool IsWarpPipe() { return !destMap.empty(); }

    void SetName(std::string n) { name = n; }
    std::string GetName() { return name; }
    void SetDestPipe(std::string dp) { destPipe = dp; }
    std::string GetDestPipe() { return destPipe; }
};
