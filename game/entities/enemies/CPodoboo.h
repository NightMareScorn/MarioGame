#ifndef __CPODOBOO_H__
#define __CPODOBOO_H__

#include "../CGameObject.h"

class CPodoboo : public CGameObject {
    float startY;
    float vy;
    int state;
public:
    enum State { JUMPING, WAITING };
    CPodoboo(float x, float y) : CGameObject(x, y) {
        this->type = "podoboo";
        this->spriteName = "ANI_FIRE_LAVA_SHOOT";
        this->startY = y;
        this->vy = 0;
        this->state = WAITING;
    }
    virtual bool IsSolid() override { return false; }
    virtual void OnCollision(CGameObject* other) override;
    virtual void Update(float dt) override;
    virtual void Render() override;
    virtual void GetBoundingBox(float& l, float& t, float& r, float& b) override;
};

#endif
