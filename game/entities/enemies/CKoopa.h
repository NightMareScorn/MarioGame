#pragma once
#include "../CGameObject.h"
#include <string>

#define KOOPA_STATE_WALKING 100
#define KOOPA_STATE_SHELL 200

class CKoopa : public CGameObject {
    std::string type;
    int state = KOOPA_STATE_WALKING;
    float dieTimer = 0.0f;
public:
    CKoopa(float x, float y, std::string type = "green_walking", float patrolLeft = 0, float patrolRight = 0);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;

    void SetState(int s) override;
    bool IsEnemy() const override { return state != KOOPA_STATE_SHELL; }
    void OnStomped() override { SetState(KOOPA_STATE_SHELL); }
};
