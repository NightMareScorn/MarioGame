#pragma once
#include "../CGameObject.h"

#define STAR_BBOX_WIDTH 14.0f
#define STAR_BBOX_HEIGHT 16.0f

#define STAR_STATE_HIDDEN 100
#define STAR_STATE_MOVING 200

#define STAR_SPEED 0.08f
#define STAR_GRAVITY 0.002f
#define STAR_BOUNCE_FORCE 0.25f

// Star power-up item. Bounces off surfaces after emerging from a Lucky Block.
class CStar : public CGameObject {
    int state;
public:
    CStar(float x, float y);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float& l, float& t, float& r, float& b) override;
    void SetState(int s);
};
