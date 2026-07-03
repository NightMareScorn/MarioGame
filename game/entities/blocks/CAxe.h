#pragma once
#include "CBlock.h"

class CAxe : public CBlock
{
public:
    CAxe(float x, float y) : CBlock(x, y) {}
    virtual bool IsSolid() override { return false; }
    virtual void OnCollision(CGameObject *obj) override;
    virtual void Update(float dt) override {}
    virtual void Render() override;
    virtual void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
};