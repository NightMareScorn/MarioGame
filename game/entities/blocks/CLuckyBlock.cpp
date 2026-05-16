#include "CLuckyBlock.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../../engine/audio/CAudioManager.h"
#include "../../entities/player/CMario.h"

CLuckyBlock::CLuckyBlock(float x, float y, bool isEmpty) : CBlock(x, y), hiddenItem(nullptr) {
    state = isEmpty ? EMPTY : IDLE;
}

void CLuckyBlock::Update(float dt) {}

void CLuckyBlock::Render() {
    std::string ani = (state == EMPTY) ? "ANI_LUCKY_BOX_EMPTY" : "ANI_LUCKY_BOX_OW_IDLE";
    CAnimations::GetInstance()->Render(ani, x, y);
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
        CAudioManager::GetInstance()->Play("bump");
        if (hiddenItem != nullptr) {
            if (auto coin = dynamic_cast<CCoin*>(hiddenItem)) {
                CAudioManager::GetInstance()->Play("coin");
                coin->SetState(COIN_STATE_POPPING);
            } else if (auto mushroom = dynamic_cast<CMushroom*>(hiddenItem)) {
                mushroom->y += 16.0f; // Start above the block
                mushroom->SetState(MUSHROOM_STATE_MOVING);
            }
        }
    }
}
