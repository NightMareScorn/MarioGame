#pragma once
#include "../CGameObject.h"

#define COIN_STATE_HIDDEN 100
#define COIN_STATE_POPPING 200
#define COIN_STATE_DONE 300

#define COIN_POP_SPEED 0.15f
#define COIN_GRAVITY 0.005f

class CCoin : public CGameObject {
    int state;
    float start_y;
    bool in_block;
public:
    CCoin(float x, float y, bool hidden_in_block = false);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float& left, float& bottom, float& right, float& top) override;
    void SetState(int s);

    bool IsBlocking(CGameObject* other) override { return false; }
    void OnCollisionX(CGameObject* other, float nx) override;
    void OnCollisionY(CGameObject* other, float ny) override;
};
