#pragma once
#include "CBlock.h"
#include <string>

enum class EBrickState
{
    IDLE,
    BOUNCING_UP,
    BOUNCING_DOWN,
    BROKEN
};

class CBrick : public CBlock
{
    EBrickState state;
    std::string aniName;
    float bounceStartY;

    bool isInvisible = false; // TODO: Làm rõ biến này

public:
    CBrick(float x, float y, const std::string &aniName = "ANI_BRICK_IDLE", bool isInvisible = false);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
    void OnHitFromBelow(CGameObject *hitter) override;

    EBrickState GetState() const { return state; }
    bool IsInvisible() { return isInvisible; }             // TODO: Xem xét lại cách sử dụng hàm
    void SetInvisible(bool value) { isInvisible = value; } // TODO: Xem xét lại cách sử dụng hàm
};