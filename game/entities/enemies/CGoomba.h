#pragma once
#include "../CGameObject.h"
#include <string>

#define GOOMBA_WALKING_SPEED 0.05f
#define GOOMBA_GRAVITY 0.002f

#define GOOMBA_STATE_WALKING 100
#define GOOMBA_STATE_DIE 200

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 15

class CGoomba : public CGameObject {
    int state;
public:
    CGoomba(float x, float y, float patrolLeft = 0, float patrolRight = 0);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &l, float &t, float &r, float &b) override;
    void SetState(int s);
};
