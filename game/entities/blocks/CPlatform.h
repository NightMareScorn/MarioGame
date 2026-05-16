#ifndef __CPLATFORM_H__
#define __CPLATFORM_H__

#include "CBridge.h"

class CPlatform : public CBridge {
    float direction;
    float speed;
    bool isVertical;
public:
    CPlatform(float x, float y, float dir = 1.0f) : CBridge(x, y, "ANI_MOVING_PLATFORM") {
        this->type = "platform";
        this->spriteName = "ANI_MOVING_PLATFORM";
        this->direction = dir;
        this->speed = 0.05f;
        this->isVertical = true;
    }
    void SetSpeed(float s) { speed = s; }
    void SetIsVertical(bool v) { isVertical = v; }
    virtual void Update(float dt) override;
    virtual void Render() override;
    virtual void GetBoundingBox(float& l, float& t, float& r, float& b) override;
};

#endif
