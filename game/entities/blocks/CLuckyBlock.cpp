#include "CLuckyBlock.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../entities/player/CMario.h"

CLuckyBlock::CLuckyBlock(float x, float y) : CBlock(x, y), state(IDLE), hiddenItem(nullptr) {}

void CLuckyBlock::Update(float dt) {}

void CLuckyBlock::Render() {
    // If empty, we can just static render one frame or stop animating, 
    // but for now, we'll keep it simple: Render same or empty sprite if provided
    // Because we don't have an empty sprite yet, we'll just render the normal box.
    // In the future, we could add ANI_EMPTY_BOX
    CAnimations::GetInstance()->Render("ANI_LUCKY_BOX_OW_IDLE", x, y);
}

void CLuckyBlock::GetBoundingBox(float &l, float &t, float &r, float &b) {
    const float BLOCK_W = 16.0f;
    const float BLOCK_H = 16.0f;
    l = x;
    t = y;
    r = x + BLOCK_W;
    b = y + BLOCK_H;
}

void CLuckyBlock::OnHitFromBelow(CGameObject* hitter) {
    if (state == IDLE) {
        state = EMPTY;
        if (hiddenItem != nullptr) {
            if (auto coin = dynamic_cast<CCoin*>(hiddenItem)) {
                coin->SetState(COIN_STATE_POPPING);
            } else if (auto mushroom = dynamic_cast<CMushroom*>(hiddenItem)) {
                mushroom->y += 16.0f; // Start above the block
                mushroom->SetState(MUSHROOM_STATE_MOVING);
            }
        }
    }
}
