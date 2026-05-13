#include "CPlayScene.h"
#include "../../registry/CResourceRegistry.h"
#include "../../registry/CMapLoader.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/utils/debug.h"
#include "../../../engine/physics/CCollision.h"

void CPlayScene::Load() {
    auto registry = CResourceRegistry::GetInstance();
    registry->LoadResourcesForPlayScene();

    mario = nullptr; // Let MapLoader initialize mario
    CMapLoader::GetInstance()->Load(levelPath, this);

    if (mario == nullptr) {
        DebugOut(L"[WARNING] Mario not found in map! Creating default.\n");
        mario = new CMario();
        mario->x = 100.0f;
        mario->y = 150.0f;
    }

    CCamera::GetInstance()->SetCamPos(0, 0);
    CCamera::GetInstance()->SetMapWidth(0); // Will be set in process map or update

    DebugOut(L"[INFO] CPlayScene::Load complete. Blocks: %d, Decors: %d\n",
             (int)blocks.size(), (int)decors.size());
}

void CPlayScene::Update(float dt) {
    mario->Update(dt);
    for (auto b : blocks) b->Update(dt);
    for (auto e : enemies) e->Update(dt);
    for (auto i : items) i->Update(dt);

    std::vector<CGameObject*> coObjectsForMario;
    coObjectsForMario.reserve(blocks.size() + enemies.size() + items.size());
    for (auto b : blocks) coObjectsForMario.push_back(b);
    for (auto e : enemies) coObjectsForMario.push_back(e);
    for (auto i : items) coObjectsForMario.push_back(i);

    CCollision::ResolveCollision(mario, dt, coObjectsForMario);

    // Resolve collision for enemies and items against blocks only
    std::vector<CGameObject*> coObjectsForOthers;
    coObjectsForOthers.reserve(blocks.size());
    for (auto b : blocks) coObjectsForOthers.push_back(b);

    for (auto e : enemies) {
        CCollision::ResolveCollision(e, dt, coObjectsForOthers);
    }
    for (auto i : items) {
        CCollision::ResolveCollision(i, dt, coObjectsForOthers);
    }

    mario->UpdateState(); 
    
    // Boundary checks for Mario
    if (mario->x < 0) mario->x = 0;
    if (mapWidth > 0 && mario->x > mapWidth - 16) mario->x = mapWidth - 16;
    mario->SetMapWidth(mapWidth);

    CCamera::GetInstance()->SetMapWidth(mapWidth);
    CCamera::GetInstance()->Update(mario->x, mario->y, (DWORD)dt);
}

void CPlayScene::Render() {
    // Render order: background decors → blocks → items → enemies → mario (front)
    for (auto d : decors) d->Render();
    for (auto b : blocks) b->Render();
    for (auto i : items) i->Render();
    for (auto e : enemies) e->Render();
    mario->Render();
}

void CPlayScene::Unload() {
    delete mario;
    mario = nullptr;

    for (auto b : blocks) delete b;
    blocks.clear();

    for (auto d : decors) delete d;
    decors.clear();

    for (auto e : enemies) delete e;
    enemies.clear();

    for (auto i : items) delete i;
    items.clear();
}
