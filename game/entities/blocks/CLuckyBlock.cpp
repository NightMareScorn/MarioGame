#include "CLuckyBlock.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../entities/player/CMario.h"

CLuckyBlock::CLuckyBlock(float x, float y) : CBlock(x, y), state(IDLE), hiddenItem(nullptr) {}

void CLuckyBlock::Update(float dt) {}

void CLuckyBlock::Render() {
    if (state == EMPTY) {
        CAnimations::GetInstance()->Render("ANI_LUCKY_BOX_OW_AFTER_HIT", x, y);
    } else {
        CAnimations::GetInstance()->Render("ANI_LUCKY_BOX_OW_IDLE", x, y);
    }
}

void CLuckyBlock::GetBoundingBox(float &left, float &bottom, float &right, float &top) {
    const float BLOCK_W = 16.0f;
    const float BLOCK_H = 16.0f;
    left = x;
    bottom = y;
    right = x + BLOCK_W;
    top = y + BLOCK_H;
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
