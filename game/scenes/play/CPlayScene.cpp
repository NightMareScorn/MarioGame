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
    // 1. XỬ LÝ CHUYỂN MAP NGAY LẬP TỨC (Hồi sinh hoặc qua màn)
    if (!pendingMapPath.empty()) {
        std::string path = pendingMapPath;
        Unload();
        Load(path);
        return; 
    }

    if (!mario) return;

    // 2. CẬP NHẬT MARIO CƠ BẢN
    mario->Update(dt);

    // 3. CẬP NHẬT ITEMS (Bao gồm cả Nấm, Hoa, Sao và Đạn lửa Fireballs)
    for (auto i : items) {
        if (i->IsDead()) continue;
        i->Update(dt);
        
        // Không tính vật lý chặn tường cho Hoa lửa (vì đứng yên) và Đạn lửa (đạn có trọng lực riêng)
        if (!dynamic_cast<CFireFlower*>(i) && !dynamic_cast<CFireball*>(i)) {
            auto blocksAround = GetObjectsInRange(i->x, i->y, blocks);
            CCollision::ResolveCollision(i, dt, blocksAround);
        }

        // Xử lý đạn lửa bâu trúng quái
        if (auto fb = dynamic_cast<CFireball*>(i)) {
            float fl, fb_bottom, fr, ft; fb->GetBoundingBox(fl, fb_bottom, fr, ft);
            for (auto e : enemies) {
                if (e->IsDead() || !e->IsEnemy()) continue;
                float el, eb, er, et; e->GetBoundingBox(el, eb, er, et);
                if (CCollision::CheckAABB({fl, fb_bottom, fr, ft}, {el, eb, er, et})) {
                    // Đạn trúng quái: diệt quái và hủy đạn
                    e->SetIsDead(true);
                    fb->Kill();
                    break;
                }
            }
        }
    }

    auto kb = KeyboardManager::GetInstance();
    
    // Nhấn phím 3 để biến thành FIRE MARIO ngay lập tức
    if (kb->IsKeyPressed('3')) {
        mario->SetPower(EMarioPower::SMALL_FIRE);
        mario->y -= 16.0f;
        DebugOut(L"[DEBUG] Cheated: Mario is now FIRE state!\n");
    }

    // 4. XỬ LÝ LOGIC MARIO KHI CHƯA CHẾT
    if (!mario->IsDeadState()) {
        // Lấy các vật thể xung quanh để tính va chạm
        std::vector<CGameObject *> coObjects;
        auto nearbyBlocks = GetObjectsInRange(mario->x, mario->y, blocks);
        auto nearbyEnemies = GetObjectsInRange(mario->x, mario->y, enemies);
        auto nearbyItems = GetObjectsInRange(mario->x, mario->y, items);
        coObjects.insert(coObjects.end(), nearbyBlocks.begin(), nearbyBlocks.end());
        coObjects.insert(coObjects.end(), nearbyEnemies.begin(), nearbyEnemies.end());
        coObjects.insert(coObjects.end(), nearbyItems.begin(), nearbyItems.end());

        // CHỈ va chạm vật lý (chặn tường/đứng sàn) khi KHÔNG đang trượt cột cờ
        if (mario->GetState() != EMarioState::GOAL_SLIDE) {
            CCollision::ResolveCollision(mario, dt, coObjects);
        }

        float ml, mb, mr, mt; mario->GetBoundingBox(ml, mb, mr, mt);

        // --- A. LOGIC ĂN VẬT PHẨM (Nấm, Hoa, Sao) ---
        for (auto i : items) {
            if (i->IsDead()) continue;
            float il, ib, ir, it; i->GetBoundingBox(il, ib, ir, it);

            if (CCollision::CheckAABB({ml, mb, mr, mt}, {il, ib, ir, it})) { 
                if (dynamic_cast<CMushroom*>(i)) {
                    mario->PowerUpMushroom(); // Biến thành BIG
                    i->SetIsDead(true);
                }
                else if (dynamic_cast<CFireFlower*>(i)) {
                    mario->PowerUpFlower(); // BIẾN THÀNH FIRE MARIO (Màu trắng/đỏ)
                    i->SetIsDead(true);
                }
            }
        }

        // --- B. LOGIC DẪM QUÁI (STOMP) ---
        for (auto e : enemies) {
            if (e->IsDead()) continue;
            float el, eb, er, et; e->GetBoundingBox(el, eb, er, et);

            if (CCollision::CheckAABB({ml, mb, mr, mt}, {el, eb, er, et})) {
                if (mario->vy < 0 && (mb > eb)) { // Đang rơi và mông > đầu quái
                    e->OnStomped();
                    mario->StompBounce();
                } else {
                    if (!mario->IsInvincible()) mario->Hurt(); // Bị thương
                }
            }
        }

        // --- C. LOGIC CỘT CỜ (Task 53) ---
        if (goalTimer <= 0 && mario->GetState() != EMarioState::GOAL_SLIDE && mario->GetState() != EMarioState::GOAL_WALK) {
            for (auto d : decors) {
                if (dynamic_cast<CFlagpole*>(d)) { // Tìm đối tượng Cột cờ
                    float dl, db, dr, dt_b; d->GetBoundingBox(dl, db, dr, dt_b);
                    if (CCollision::CheckAABB({ml, mb, mr, mt}, {dl, db, dr, dt_b})) {
                        
                        // Khóa Mario vào thân cột và kích hoạt trượt xuống
                        mario->x = dl - 4.0f; 
                        mario->HitGoal();
                        goalTimer = 10.0f; // Chờ 10s để trượt và đi vào lâu đài
                        DebugOut(L"[INFO] Flagpole hit! Transition in 10s.\n");
                        break;
                    }
                }
            }
        }

        // Đếm ngược thời gian thắng màn để chuyển Map
        if (goalTimer > 0) {
          goalTimer -= dt;

        if (mario->GetState() == EMarioState::GOAL_SLIDE) {
        // Mario trượt xuống cho đến khi chạm đất
        mario->vy = -0.06f; 
        if (mario->y <= 32.0f) { // 32.0f là groundY của bạn
            mario->y = 32.0f;
            mario->SetState(static_cast<int>(EMarioState::GOAL_WALK)); // Chuyển sang đi bộ vào lâu đài
        }
    } else if (mario->GetState() == EMarioState::GOAL_WALK) {
        mario->vx = 0.05f; // Đi bộ tự động
        mario->x += mario->vx * dt;
    }

    if (goalTimer <= 0) {
        // Hết thời gian chờ -> Chuyển Map
        this->TransitionToMap("content/levels/level_1_2_first_half.csv");
    }
    return; // Thoát sớm để không bị rớt vực hay va chạm quái khi đã thắng
}

        // --- D. KIỂM TRA RỚT VỰC (Chỉ kiểm tra khi chưa thắng) ---
        if (mario->y < -64.0f && goalTimer <= 0) {
            mario->Die();
        }

        // --- E. CẬP NHẬT CHECKPOINT ---
        if (mario->x > 1000.0f && !CMario::hasCheckpoint) {
            CMario::hasCheckpoint = true;
            CMario::checkpointX = 1000.0f;
            CMario::checkpointY = 100.0f;
        }
    } 
    else {
        // XỬ LÝ HỒI SINH KHI CHẾT XONG ( dieTimer về 0)
        if (mario->GetDieTimer() <= 0) {
            if (CMario::lives > 0) this->TransitionToMap(currentMapPath);
            else {
                CMario::lives = 3; CMario::hasCheckpoint = false;
                this->TransitionToMap("content/levels/level_1_1.csv");
            }
            return;
        }
    }

    // 5. CẬP NHẬT QUÁI VẬT
    for (auto e : enemies) {
        if (e->IsDead()) continue;
        e->Update(dt);
        auto blocksAround = GetObjectsInRange(e->x, e->y, blocks);
        CCollision::ResolveCollision(e, dt, blocksAround);
    }

    // 6. DỌN DẸP DANH SÁCH (Items/Đạn/Quái đã chết)
    items.erase(std::remove_if(items.begin(), items.end(), [](CGameObject* o) { 
        if (o->IsDead()) { delete o; return true; } return false; 
    }), items.end());

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](CGameObject* o) { 
        if (o->IsDead()) { delete o; return true; } return false; 
    }), enemies.end());

    // 7. CẬP NHẬT TRẠNG THÁI CUỐI VÀ CAMERA
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