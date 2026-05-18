#include "CLuckyBlock.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../entities/player/CMario.h"
#include "../../scenes/play/CPlayScene.h"
#include "../../entities/items/CFireFlower.h"
#include "../../entities/items/CMushroom.h"
#include "../../scenes/CSceneManager.h"

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

void CLuckyBlock::OnHitFromBelow(CGameObject *hitter) {
    if (this->state == EMPTY) return; 
    this->state = EMPTY;

    CMario* mario = dynamic_cast<CMario*>(hitter);
    CPlayScene* scene = (CPlayScene*)CSceneManager::GetInstance()->GetCurrentScene();
    
    CGameObject* item = nullptr;
    bool isNewItem = false;

    if (this->hiddenItem != nullptr) {
        item = this->hiddenItem;
        // Nếu là Nấm mà Mario đã lớn -> Tạo Hoa mới
        if (dynamic_cast<CMushroom*>(item) && mario && mario->GetPower() != EMarioPower::SMALL) {
            item = new CFireFlower(x, y);
            isNewItem = true;
        }
    } else {
        if (mario && mario->GetPower() == EMarioPower::SMALL) 
            item = new CMushroom(x, y);
        else 
            item = new CFireFlower(x, y);
        isNewItem = true;
    }

    if (item) {
        item->SetPosition(this->x, this->y + 18.0f); 
        if (isNewItem && scene) scene->AddItem(item);
        item->SetState(200); 
    }
    
    this->hiddenItem = nullptr; 
}
