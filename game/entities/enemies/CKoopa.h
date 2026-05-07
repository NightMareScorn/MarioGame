#pragma once
#include "../CGameObject.h"

#define KOOPA_BBOX_WIDTH 16.0f
#define KOOPA_BBOX_HEIGHT 24.0f

#define KOOPA_STATE_WALKING 100
#define KOOPA_STATE_SHELL 200

#define KOOPA_WALKING_SPEED 0.05f
#define KOOPA_GRAVITY 0.002f

class CKoopa : public CGameObject {
    float start_x;
    int state;
public:
    CKoopa(float x, float y);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float& l, float& t, float& r, float& b) override;
    void SetState(int s);
};
