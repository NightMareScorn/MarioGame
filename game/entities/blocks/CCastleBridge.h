#ifndef __CCASTLEBRIDGE_H__
#define __CCASTLEBRIDGE_H__

#include "CBlock.h"

class CCastleBridge : public CBlock {
public:
    CCastleBridge(float x, float y) : CBlock(x, y) {
        this->type = "castle_bridge";
        this->spriteName = "ANI_BRIDGE_BLOCK";
    }
    virtual void Update(float dt) override;
    virtual void Render() override;
    virtual void GetBoundingBox(float& l, float& t, float& r, float& b) override;
};

#endif
