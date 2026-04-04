#include "CPlayScene.h"
#include "../../registry/CResourceRegistry.h"
#include "../../../engine/rendering/Camera.h"
#include "../../entities/blocks/CBrick.h"
#include "../../../engine/utils/debug.h"
#include "../../../engine/physics/CCollision.h"

void CPlayScene::Load() {
    auto registry = CResourceRegistry::GetInstance();
    registry->LoadResourcesForPlayScene();

    mario = new CMario();
    mario->x = 100.0f;
    mario->y = 150.0f; // Di chuyển Mario lại gần đầu map để xem cột

    // 1. Mặt đất cơ bản (dài 45 ô)
    for (int i = 0; i < 45; i++) {
        blocks.push_back(new CBrick(i * 16.0f, 50.0f));
    }

    // 2. Cột thẳng đứng bên trái (ở vị trí x = 0, chồng cao lên 7 ô)
    for (int i = 1; i <= 7; i++) {
        blocks.push_back(new CBrick(0.0f, 50.0f + i * 16.0f));
    }

    // 3. Cầu thang 5 bậc bên phải (bắt đầu từ cột 20)
    int stairStartX = 20;
    for (int step = 1; step <= 5; step++) {
        for (int h = 1; h <= step; h++) {
            blocks.push_back(new CBrick((stairStartX + step - 1) * 16.0f, 50.0f + h * 16.0f));
        }
    }

    // 4. Đoạn thẳng 5 ô nối sau bậc cao nhất của cầu thang
    int flatStartX = stairStartX + 5;
    for (int i = 0; i < 5; i++) {
        for (int h = 1; h <= 5; h++) {
            blocks.push_back(new CBrick((flatStartX + i) * 16.0f, 50.0f + h * 16.0f));
        }
    }
    DebugOut(L"[INFO] CPlayScene::Load complete. Blocks: %d\n", blocks.size());
}

void CPlayScene::Update(float dt) {
    mario->Update(dt);
    for (auto b : blocks) b->Update(dt);

    std::vector<CGameObject*> coObjects;
    coObjects.reserve(blocks.size());
    for (auto b : blocks) coObjects.push_back(b);
    CCollision::ResolveCollision(mario, dt, coObjects);
    CCamera::GetInstance()->Update(mario->x, mario->y, (DWORD)dt);
}

void CPlayScene::Render() {
    mario->Render();
    for (auto b : blocks) b->Render();
}

void CPlayScene::Unload() {
    delete mario;
    for (auto b : blocks) delete b;
    blocks.clear();
}
