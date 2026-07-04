#pragma once
#include "../CGameObject.h"

class CBlock : public CGameObject
{
public:
    CBlock(float x, float y);
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
};