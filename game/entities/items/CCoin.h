#pragma once
#include "../CGameObject.h"

#define COIN_STATE_HIDDEN 100
#define COIN_STATE_POPPING 200
#define COIN_STATE_DONE 300
#define COIN_STATE_COLLECTED 400

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
    void GetBoundingBox(float& l, float& t, float& r, float& b) override;
    void SetState(int s);
    bool IsSolid() override { return false; }
    void OnCollision(CGameObject* other) override;
};
