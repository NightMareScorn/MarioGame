#ifndef __CBOWSER_H__
#define __CBOWSER_H__

#include "../CGameObject.h"

class CBowser : public CGameObject {
public:
    CBowser(float x, float y) : CGameObject(x, y) {
        this->type = "bowser";
        this->spriteName = "ANI_BOWSER_IDLE";
    }
    virtual void Update(float dt) override;
    virtual void Render() override;
    virtual void GetBoundingBox(float& l, float& t, float& r, float& b) override;
};

#endif
