#ifndef __CFIREFLOWER_H__
#define __CFIREFLOWER_H__

#include "../CGameObject.h"

class CFireFlower : public CGameObject {
public:
    CFireFlower(float x, float y) : CGameObject(x, y) {
        this->type = "flower";
        this->spriteName = "ANI_FLOWER_OW";
    }
    virtual void Update(float dt) override;
    virtual void Render() override;
    virtual void GetBoundingBox(float& l, float& t, float& r, float& b) override;
};

#endif
