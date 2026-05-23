#pragma once
#include "CBlock.h"
#include <string>

enum class ELuckyBlockState
{
    IDLE,
    BOUNCING_UP,
    BOUNCING_DOWN,
    EMPTY
};

class CGameObject;

class CLuckyBlock : public CBlock
{
    ELuckyBlockState state;
    CGameObject* hiddenItem;
    float bounceStartY;

    void _SpawnHiddenItem();

public:
    CLuckyBlock(float x, float y, bool isEmpty = false);
    void SetHiddenItem(CGameObject* item) { hiddenItem = item; }
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float& left, float& bottom, float& right, float& top) override;
    void OnHitFromBelow(CGameObject* hitter) override;

    ELuckyBlockState GetState() const { return state; }
};
