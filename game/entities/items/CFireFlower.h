#pragma once
#include "../CGameObject.h"

#define FLOWER_BBOX_WIDTH 16.0f
#define FLOWER_BBOX_HEIGHT 16.0f

#define FLOWER_STATE_HIDDEN 100
#define FLOWER_STATE_VISIBLE 200

// Fire Flower power-up item. Stays stationary after emerging from a Lucky Block.
class CFireFlower : public CGameObject {
    int state;
public:
    CFireFlower(float x, float y);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float& left, float& bottom, float& right, float& top) override;
    void SetState(int s);
};
