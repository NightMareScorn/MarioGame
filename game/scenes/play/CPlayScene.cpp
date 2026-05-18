#include "CPlayScene.h"
#include "../../../engine/core/Game.h"
#include "../../../engine/input/KeyboardManager.h"
#include "../../../engine/physics/CCollision.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/utils/debug.h"
#include "../../entities/blocks/CPipe.h"
#include "../../entities/items/CFireFlower.h"
#include "../../entities/items/CMushroom.h"
#include "../../registry/CMapLoader.h"
#include "../../registry/CResourceRegistry.h"
#include <algorithm>

void CPlayScene::Load(const std::string &mapPath) {
    CResourceRegistry::GetInstance()->LoadResourcesForPlayScene();
    
    // Lưu lại đường dẫn map hiện tại để dùng khi hồi sinh
    this->currentMapPath = mapPath;
    this->pendingMapPath = "";

    this->mario = nullptr;
    CMapLoader::GetInstance()->Load(mapPath, this);

    // Dự phòng nếu MapLoader không tìm thấy object Mario trong CSV
    if (this->mario == nullptr) {
        this->mario = new CMario();
        this->mario->x = 100.0f;
        this->mario->y = 100.0f;
    }

    DebugOut(L"[INFO] Map Loaded: %hs. Mario at: %.2f, %.2f\n", mapPath.c_str(), mario->x, mario->y);
}

void CPlayScene::Update(float dt) {
    // 1. Xử lý nạp map ngay lập tức (Hồi sinh)
    if (!pendingMapPath.empty()) {
        std::string path = pendingMapPath;
        Unload();
        Load(path);
        return; 
    }

    if (!mario) return;
    mario->Update(dt);

    // 2. Cập nhật Items
    for (auto i : items) {
        if (i->IsDead()) continue;
        i->Update(dt);
        if (!dynamic_cast<CFireFlower*>(i)) {
            auto blocksAround = GetObjectsInRange(i->x, i->y, blocks);
            CCollision::ResolveCollision(i, dt, blocksAround);
        }
    }

    // 3. Xử lý Mario
    if (!mario->IsDeadState()) {
        std::vector<CGameObject *> coObjects;
        auto nearbyBlocks = GetObjectsInRange(mario->x, mario->y, blocks);
        auto nearbyEnemies = GetObjectsInRange(mario->x, mario->y, enemies);
        auto nearbyItems = GetObjectsInRange(mario->x, mario->y, items);
        coObjects.insert(coObjects.end(), nearbyBlocks.begin(), nearbyBlocks.end());
        coObjects.insert(coObjects.end(), nearbyEnemies.begin(), nearbyEnemies.end());
        coObjects.insert(coObjects.end(), nearbyItems.begin(), nearbyItems.end());

        // Va chạm vật lý (Chặn tường/Đứng sàn)
        CCollision::ResolveCollision(mario, dt, coObjects);

        // --- SỬ DỤNG CCollision::CheckAABB ĐỂ ĂN ITEM ---
        float ml, mb, mr, mt;
        mario->GetBoundingBox(ml, mb, mr, mt);
        
        for (auto i : items) {
            if (i->IsDead()) continue;
            float il, ib, ir, it;
            i->GetBoundingBox(il, ib, ir, it);

            // Gọi hàm CheckAABB từ class CCollision của bạn
            if (CCollision::CheckAABB({ml, mb, mr, mt}, {il, ib, ir, it})) { 
                if (dynamic_cast<CMushroom*>(i)) {
                    mario->GrowToBig(); 
                    i->SetIsDead(true);
                    DebugOut(L"[ITEM] Mario ate Mushroom!\n");
                }
                else if (dynamic_cast<CFireFlower*>(i)) {
                    mario->SetPower(EMarioPower::FIRE);
                    i->SetIsDead(true);
                }
            }
        }

        // --- SỬ DỤNG CCollision::CheckAABB ĐỂ DẪM QUÁI ---
        for (auto e : enemies) {
            if (e->IsDead()) continue;
            float el, eb, er, et;
            e->GetBoundingBox(el, eb, er, et);

            if (CCollision::CheckAABB({ml, mb, mr, mt}, {el, eb, er, et})) {
                if (mario->vy < 0 && (mb > eb)) { // Stomp
                    e->OnStomped();
                    mario->StompBounce();
                } else {
                    if (!mario->IsInvincible()) mario->Hurt();
                }
            }
        }

        if (mario->y < -64.0f) mario->Die();

        if (mario->x > 1000.0f && !CMario::hasCheckpoint) {
            CMario::hasCheckpoint = true;
            CMario::checkpointX = 1000.0f;
            CMario::checkpointY = 100.0f;
        }
    } 
    else {
        // Xử lý hồi sinh
        if (mario->GetDieTimer() <= 0) {
            if (CMario::lives > 0) this->TransitionToMap(currentMapPath);
            else {
                CMario::lives = 3; CMario::hasCheckpoint = false;
                this->TransitionToMap("content/levels/level_1_1.csv");
            }
            return;
        }
    }

    // 4. Cập nhật Enemies
    for (auto e : enemies) {
        if (e->IsDead()) continue;
        e->Update(dt);
        auto blocksAround = GetObjectsInRange(e->x, e->y, blocks);
        CCollision::ResolveCollision(e, dt, blocksAround);
    }

    // 5. Dọn dẹp object
    items.erase(std::remove_if(items.begin(), items.end(), [](CGameObject* o) { 
        if (o->IsDead()) { delete o; return true; } return false; 
    }), items.end());

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](CGameObject* o) { 
        if (o->IsDead()) { delete o; return true; } return false; 
    }), enemies.end());

    mario->UpdateState();
    CCamera::GetInstance()->Update(mario->x, mario->y, (DWORD)dt);
}

void CPlayScene::Render() {
    ID3DX10Sprite *spriteHandler = CGame::GetInstance()->GetSpriteHandler();
    for (auto d : decors) d->Render();
    spriteHandler->Flush();

    for (auto b : blocks) b->Render();
    for (auto i : items) if (!i->IsDead()) i->Render();
    for (auto e : enemies) if (!e->IsDead()) e->Render();
    if (mario) mario->Render();
    spriteHandler->Flush();
}

void CPlayScene::Unload() {
    if (mario) { delete mario; mario = nullptr; }
    for (auto b : blocks) delete b; blocks.clear();
    for (auto d : decors) delete d; decors.clear();
    for (auto e : enemies) delete e; enemies.clear();
    for (auto i : items) delete i; items.clear();
}