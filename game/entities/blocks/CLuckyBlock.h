#pragma once
#include "CBlock.h"
#include "../../entities/player/CMario.h"
#include "../../entities/items/CCoin.h"
#include "../../entities/items/CMushroom.h"

class CLuckyBlock : public CBlock {
    int state;
    CGameObject* hiddenItem;
public:
    enum State { IDLE, EMPTY };
    CLuckyBlock(float x, float y);
    void SetHiddenItem(CGameObject* item) { hiddenItem = item; }
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
    void OnHitFromBelow(CGameObject* hitter) override;
};
