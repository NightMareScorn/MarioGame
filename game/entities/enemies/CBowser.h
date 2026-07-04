#ifndef __CBOWSER_H__
#define __CBOWSER_H__

#include "../CGameObject.h"

class CBowser : public CGameObject
{
    float startX;
    float fireTimer;
    float jumpTimer;
    float nextJumpTime;
    bool isOnGround;

public:
    CBowser(float x, float y) : CGameObject(x, y)
    {
        this->type = "bowser";
        this->spriteName = "ANI_BOWSER_IDLE";
        this->startX = x;
        this->vx = -0.015f;
        this->vy = 0.0f;
        this->fireTimer = 0.0f;
        this->jumpTimer = 0.0f;
        this->nextJumpTime = 1000.0f + (float)(rand() % 2001);
        this->isOnGround = true;
    }
    virtual void Update(float dt) override;
    virtual void Render() override;
    virtual void GetBoundingBox(float &l, float &t, float &r, float &b) override;
    virtual void OnCollisionX(CGameObject *other, float nx) override;
    virtual void OnCollisionY(CGameObject *other, float ny) override;
};

#endif
