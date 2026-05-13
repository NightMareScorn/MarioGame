#ifndef __CPODOBOO_H__
#define __CPODOBOO_H__

#include "../CGameObject.h"

class CPodoboo : public CGameObject {
public:
    CPodoboo(float x, float y) : CGameObject(x, y) {
        this->type = "podoboo";
        this->spriteName = "ANI_FIRE_BAR"; // Use fire bar orb as proxy for now
    }
    virtual void Update(float dt) override;
    virtual void Render() override;
    virtual void GetBoundingBox(float& l, float& t, float& r, float& b) override;
};

#endif
