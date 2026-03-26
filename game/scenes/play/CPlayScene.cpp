#include "CPlayScene.h"
#include "../../registry/CResourceRegistry.h"
#include "../../../engine/rendering/Camera.h"
#include "../../entities/blocks/CBrick.h"
#include "../../../engine/utils/debug.h"

void CPlayScene::Load() {
    auto registry = CResourceRegistry::GetInstance();
    registry->LoadResourcesForPlayScene();

    mario = new CMario();
    mario->x = 300.0f;
    mario->y = 100.0f;

    for (int i = 0; i < 20; i++) {
        blocks.push_back(new CBrick(i * 16.02f, 50.02f));
    }
    DebugOut(L"[INFO] CPlayScene::Load complete. Blocks: %d\n", blocks.size());
}

void CPlayScene::Update(float dt) {
    mario->Update(dt);
    for (auto b : blocks) b->Update(dt);
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
