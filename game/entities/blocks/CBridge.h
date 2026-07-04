#pragma once
#include "CBlock.h"
#include <string>

// Bridge: one-way platform (only blocks from top), renders in foreground (over Mario)
class CBridge : public CBlock {
    std::string aniName;
public:
    CBridge(float x, float y, std::string aniName = "ANI_BRIDGE_BLOCK") 
        : CBlock(x, y), aniName(aniName) {}
    
    void Update(float dt) override {}
    void Render() override;
    void GetBoundingBox(float& l, float& t, float& r, float& b) override;
    std::string GetAniName() const { return aniName; }
    
    bool isOneWay = true; // Only collide from top
};
