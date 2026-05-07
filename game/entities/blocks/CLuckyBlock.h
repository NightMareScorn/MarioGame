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
    void GetBoundingBox(float &l, float &t, float &r, float &b) override;
    void OnHitFromBelow(CGameObject* hitter) override;
};
