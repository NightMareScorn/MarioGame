#pragma once
#include "CBlock.h"

// Used to block movement without rendering anything (e.g. for Pipes or invisible map boundaries)
class CInvisibleBlock : public CBlock {
    float width, height;
public:
    CInvisibleBlock(float x, float y, float w = 16.0f, float h = 16.0f) : CBlock(x, y), width(w), height(h) {}
    void Update(float dt) override {}
    void Render() override {} // Do nothing
    void GetBoundingBox(float &l, float &t, float &r, float &b) override {
        l = x;
        t = y;
        r = x + width;
        b = y + height;
    }
};
