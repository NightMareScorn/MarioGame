#pragma once
#include "CBlock.h"

class CBrick : public CBlock {
public:
    CBrick(float x, float y) : CBlock(x, y) {}
    void Update(float dt) override {}
    void Render() override;
};
