#include "CPlayScene.h"
#include "../../../engine/core/Game.h"
#include "../../../engine/input/KeyboardManager.h"
#include "../../../engine/physics/CCollision.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/utils/debug.h"
#include "../../entities/blocks/CPipe.h"
#include "../../entities/items/CFireBall.h"
#include "../../entities/items/CFireFlower.h"
#include "../../entities/items/CMushroom.h"
#include "../../entities/blocks/CFlagpole.h"
#include "../../registry/CMapLoader.h"
#include "../../registry/CResourceRegistry.h"
#include <algorithm>

void CPlayScene::Load(const std::string &mapPath) {
    CResourceRegistry::GetInstance()->LoadResourcesForPlayScene();
    
    // Lưu lại đường dẫn map hiện tại để dùng khi hồi sinh
    this->currentMapPath = mapPath;
    this->pendingMapPath = "";
    this->goalTimer = 0.0f;

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
    if (!pendingMapPath.empty()) {
        std::string path = pendingMapPath;
        Unload(); Load(path); return; 
    }

    if (!mario) return;

    // 1. Cập nhật lá cờ và Mario tự động khi thắng
    for (auto d : decors) d->Update(dt);

    if (goalTimer > 0) {
        goalTimer -= dt;
        if (mario->GetState() == EMarioState::GOAL_SLIDE) {
            if (mario->y <= 32.5f) { // Chạm đất
                mario->y = 32.0f;
                mario->SetState(static_cast<int>(EMarioState::GOAL_WALK));
            }
        }
        mario->Update(dt); // Chạy logic Slide hoặc Walk trong CMario
        mario->SetOnGround(true);
        CCamera::GetInstance()->Update(mario->x, mario->y, (DWORD)dt);
        if (goalTimer <= 0) {
            CMario::hasCheckpoint = false;
            this->TransitionToMap("content/levels/level_1_2_first_half.csv");
        }
        return; 
    }

    // 2. Cập nhật bình thường
    mario->Update(dt);

    for (auto i : items) {
        if (i->IsDead()) continue;
        i->Update(dt);
        if (!dynamic_cast<CFireFlower*>(i) && !dynamic_cast<CFireball*>(i)) {
            auto blocksAround = GetObjectsInRange(i->x, i->y, blocks);
            CCollision::ResolveCollision(i, dt, blocksAround);
        }
    }

    // 3. Xử lý va chạm
    if (!mario->IsDeadState()) {
        std::vector<CGameObject *> coObjects;
        auto nearbyBlocks = GetObjectsInRange(mario->x, mario->y, blocks);
        auto nearbyEnemies = GetObjectsInRange(mario->x, mario->y, enemies);
        auto nearbyItems = GetObjectsInRange(mario->x, mario->y, items);
        coObjects.insert(coObjects.end(), nearbyBlocks.begin(), nearbyBlocks.end());
        coObjects.insert(coObjects.end(), nearbyEnemies.begin(), nearbyEnemies.end());
        coObjects.insert(coObjects.end(), nearbyItems.begin(), nearbyItems.end());

        // ResolveCollision của team bạn sẽ handle việc cộng x, y
        CCollision::ResolveCollision(mario, dt, coObjects);

        float ml, mb, mr, mt; mario->GetBoundingBox(ml, mb, mr, mt);

        // Ăn Item & Dẫm quái
        for (auto i : items) {
            if (i->IsDead()) continue;
            float il, ib, ir, it; i->GetBoundingBox(il, ib, ir, it);
            if (CCollision::CheckAABB({ml, mb, mr, mt}, {il, ib, ir, it})) { 
                if (dynamic_cast<CMushroom*>(i)) { mario->PowerUpMushroom(); i->SetIsDead(true); }
                else if (dynamic_cast<CFireFlower*>(i)) { mario->PowerUpFlower(); i->SetIsDead(true); }
            }
        }
        for (auto e : enemies) {
    if (e->IsDead()) continue;
    float el, eb, er, et; e->GetBoundingBox(el, eb, er, et);
    float ml, mb, mr, mt; mario->GetBoundingBox(ml, mb, mr, mt);

    if (CCollision::CheckAABB({ml, mb, mr, mt}, {el, eb, er, et})) {
        
        // 3. LOGIC STAR MODE: Chạm vào là quái chết luôn
        if (mario->IsStarMode()) {
            e->OnStomped(); // Hoặc gọi hàm chết trực tiếp của quái
            DebugOut(L"[STAR MODE] Enemy killed by contact!\n");
        } 
        else {
            // Logic Stomp và Hurt bình thường
            if (mario->vy < 0 && (mb > eb)) { 
                e->OnStomped(); 
                mario->StompBounce(); 
            } else {
                if (!mario->IsInvincible()) mario->Hurt();
            }
        }
    }
}

        // Logic Flagpole
        if (!mario->IsInputLocked()) {
            for (auto d : decors) {
                if (dynamic_cast<CFlagpole*>(d)) { 
                    float dl, db, dr, dt_b; d->GetBoundingBox(dl, db, dr, dt_b);
                    if (CCollision::CheckAABB({ml, mb, mr, mt}, {dl, db, dr, dt_b})) {
                        mario->x = dl - 4.0f;
                        mario->HitGoal();
                        d->SetState(200); // Hạ cờ
                        goalTimer = 4000.0f; 
                        break;
                    }
                }
            }
        }
        if (mario->y < -64.0f) mario->Die();
    } else {
        if (mario->GetDieTimer() <= 0) {
            if (CMario::lives > 0) this->TransitionToMap(currentMapPath);
            else { CMario::lives = 3; CMario::hasCheckpoint = false; this->TransitionToMap("content/levels/level_1_1.csv"); }
            return;
        }
    }

    for (auto e : enemies) {
        if (e->IsDead()) continue;
        e->Update(dt);
        auto blocksAround = GetObjectsInRange(e->x, e->y, blocks);
        CCollision::ResolveCollision(e, dt, blocksAround);
    }
    items.erase(std::remove_if(items.begin(), items.end(), [](CGameObject* o) { if (o->IsDead()) { delete o; return true; } return false; }), items.end());
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