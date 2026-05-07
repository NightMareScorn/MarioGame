#include "CPlayScene.h"
#include "../../registry/CResourceRegistry.h"
#include "../../registry/CMapLoader.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/utils/debug.h"
#include "../../../engine/physics/CCollision.h"
#include "../../../engine/core/Game.h"

void CPlayScene::Load() {
    auto registry = CResourceRegistry::GetInstance();
    registry->LoadResourcesForPlayScene();

    mario = nullptr; // Let MapLoader initialize mario
    CMapLoader::GetInstance()->Load("content/levels/level_1_1.csv", this);

    if (mario == nullptr) {
        DebugOut(L"[WARNING] Mario not found in map! Creating default.\n");
        mario = new CMario();
        mario->x = 100.0f;
        mario->y = 150.0f;
    }

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
    // In a real game, enemies and items will have their own collision callbacks with Mario
    for (auto e : enemies) coObjectsForMario.push_back(e);
    for (auto i : items) coObjectsForMario.push_back(i);

    CCollision::ResolveCollision(mario, dt, coObjectsForMario);

    // Resolve collision for enemies and block-colliding items too, against blocks only
    std::vector<CGameObject*> coObjectsForOthers;
    coObjectsForOthers.reserve(blocks.size());
    for (auto b : blocks) coObjectsForOthers.push_back(b);

    for (auto e : enemies) {
        CCollision::ResolveCollision(e, dt, coObjectsForOthers);
    }
    for (auto i : items) {
        CCollision::ResolveCollision(i, dt, coObjectsForOthers);
    }

    std::vector<CGameObject*> coObjects;
    coObjects.reserve(blocks.size());
    for (auto b : blocks) coObjects.push_back(b);
    CCollision::ResolveCollision(mario, dt, coObjects);
    mario->UpdateState(); 
    CCamera::GetInstance()->Update(mario->x, mario->y, (DWORD)dt);
}

void CPlayScene::Render() {
    ID3DX10Sprite* spriteHandler = CGame::GetInstance()->GetSpriteHandler();
    ID3D10Device* pD3DDevice = CGame::GetInstance()->GetDirect3DDevice();
    FLOAT NewBlendFactor[4] = { 0,0,0,0 };

    // LAYER 1: Background decorations (hills, clouds, bushes, flag, castle)
    // Rendered first so they appear BEHIND everything else.
    for (auto d : decors) d->Render();

    // Flush the decor layer to GPU before rendering foreground objects.
    // D3DX10_SPRITE_SORT_TEXTURE reorders sprites by texture within a batch,
    // which can cause later-drawn sprites to appear behind earlier ones.
    // By flushing here, we guarantee decors are committed to the framebuffer first.
    spriteHandler->Flush();

    // LAYER 2: Foreground — blocks, items, enemies, mario (all in same batch)
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
