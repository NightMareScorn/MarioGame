#pragma once
#include "../CGameObject.h"
#include <string>

#define GOOMBA_WALKING_SPEED 0.05f
#define GOOMBA_GRAVITY 0.002f

#define GOOMBA_STATE_WALKING 100
#define GOOMBA_STATE_DIE 200

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 15
#define GOOMBA_BBOX_HEIGHT_DIE 7

class CGoomba : public CGameObject
{
protected:
    float patrolLeft, patrolRight;
    int state;
    float dieTimer = 0.0f;

public:
    CGoomba(float x, float y, float patrolLeft = 0, float patrolRight = 0);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
    void SetState(int s);

    bool IsBlocking(CGameObject *other) override;
    void OnCollisionX(CGameObject *other, float nx) override;
    void OnCollisionY(CGameObject *other, float ny) override;

    bool IsEnemy() const override { return state != GOOMBA_STATE_DIE; }
    void OnStomped() override { SetState(GOOMBA_STATE_DIE); }
};
