#ifndef __CPLATFORM_H__
#define __CPLATFORM_H__

#include "CBlock.h"

class CPlatform : public CBlock {
    float direction;
public:
    CPlatform(float x, float y, float dir = 1.0f) : CBlock(x, y) {
        this->type = "platform";
        this->spriteName = "ANI_MUSHROOM_PLATFORM_MID";
        this->direction = dir;
    }
    virtual void Update(float dt) override;
    virtual void Render() override;
    virtual void GetBoundingBox(float& l, float& t, float& r, float& b) override;
};

#endif
