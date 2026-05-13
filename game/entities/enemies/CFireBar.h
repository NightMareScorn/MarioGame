#ifndef __CFIREBAR_H__
#define __CFIREBAR_H__

#include "../CGameObject.h"

class CFireBar : public CGameObject {
public:
    CFireBar(float x, float y) : CGameObject(x, y) {
        this->type = "firebar";
        this->spriteName = "ANI_FIRE_BAR";
    }
    virtual void Update(float dt) override;
    virtual void Render() override;
    virtual void GetBoundingBox(float& l, float& t, float& r, float& b) override;
};

#endif
