#ifndef __CFIREBAR_H__
#define __CFIREBAR_H__

#include "../CGameObject.h"

class CFireBar : public CGameObject {
    float centerX, centerY;
    float offset;
    float angle;
public:
    CFireBar(float x, float y) : CGameObject(x, y) {
        this->type = "firebar";
        this->spriteName = "ANI_FIRE_BAR_ORB";
        this->centerX = x;
        this->centerY = y;
        this->offset = 0;
        this->angle = 0;
    }
    void SetOffset(float o) { offset = o; }
    void SetAngle(float a) { angle = a; }
    virtual bool IsSolid() override { return false; }
    virtual void OnCollision(CGameObject* other) override;
    virtual void Update(float dt) override;
    virtual void Render() override;
    virtual void GetBoundingBox(float& l, float& t, float& r, float& b) override;
};

#endif
