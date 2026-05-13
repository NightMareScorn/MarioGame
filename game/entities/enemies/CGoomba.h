#pragma once
#include "../CGameObject.h"

#define GOOMBA_BBOX_WIDTH 16.0f
#define GOOMBA_BBOX_HEIGHT 15.0f

#define GOOMBA_STATE_WALKING 100
#define GOOMBA_STATE_DIE 200

#define GOOMBA_WALKING_SPEED 0.05f
#define GOOMBA_GRAVITY 0.002f

class CGoomba : public CGameObject {
protected:
    float start_x;
    int state;
public:
    CGoomba(float x, float y);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float& l, float& t, float& r, float& b) override;
    void SetState(int s);
};
