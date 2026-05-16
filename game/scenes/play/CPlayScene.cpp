#include "CPlayScene.h"
#include "../../registry/CResourceRegistry.h"
#include "../../registry/CMapLoader.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/utils/debug.h"
#include "../../../engine/physics/CCollision.h"
#include "../../../engine/utils/CScoreManager.h"
#include "../../../engine/core/Game.h"
#include <algorithm>

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
    for (auto b : blocks) b->Update(dt);
    for (auto e : enemies) e->Update(dt);
    for (auto i : items) i->Update(dt);
    for (auto d : decors) d->Update(dt);

    // Sync Mario with moving platforms
    for (auto b : blocks) {
        if (b->type == "platform") {
            float ml, mt, mr, mb;
            mario->GetBoundingBox(ml, mt, mr, mb);
            float pl, pt, pr, pb;
            b->GetBoundingBox(pl, pt, pr, pb);

            // Stickiness: if Mario is on top OR just above the platform while it moves down
            if (mr > pl && ml < pr && mb >= pb && mb <= pb + 4.0f && mario->vy <= 0) {
                mario->x += b->vx * dt;
                mario->y += b->vy * dt;
                
                // If the platform is moving down, snap Mario to it to keep him on ground
                if (b->vy < 0) {
                    mario->y = pb + 0.1f;
                    mario->SetOnGround(true);
                }
            }
        }
    }

    mario->Update(dt);

    std::vector<CGameObject*> coObjectsForMario;
    coObjectsForMario.reserve(blocks.size() + enemies.size() + items.size());
    for (auto b : blocks) coObjectsForMario.push_back(b);
    for (auto e : enemies) coObjectsForMario.push_back(e);
    for (auto i : items) coObjectsForMario.push_back(i);
    
    if (mario->GetState() != EMarioState::DIE) {
        CCollision::ResolveCollision(mario, dt, coObjectsForMario);
    }

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

    if (mario->IsDead()) {
        Unload();
        Load();
        return;
    }

    CCamera::GetInstance()->SetMapWidth(mapWidth);
    CCamera::GetInstance()->Update(mario->x, mario->y, (DWORD)dt);

    // Check for level completion (near end castle)
    if (mapWidth > 0 && mario->x > mapWidth - 64.0f) {
        std::string levelId = "1-1";
        size_t lastSlash = levelPath.find_last_of("\\/");
        std::string fileName = (lastSlash == std::string::npos) ? levelPath : levelPath.substr(lastSlash + 1);
        if (fileName.find("level_") == 0) {
            levelId = fileName.substr(6, 3);
            std::replace(levelId.begin(), levelId.end(), '_', '-');
        }

        // Dummy score/time for now, could be calculated based on coins/enemies/time
        CScoreManager::GetInstance()->UpdateBest(levelId, 80000, "01:23");
        CGame::GetInstance()->SetExitLevel(true);
    }
}

void CPlayScene::Render() {
    // Render order: background decors → blocks (skip bridges) → items → enemies → mario → foreground (bridges)
    for (auto d : decors) d->Render();
    for (auto b : blocks) {
        // Skip bridges in normal pass - they render in foreground
        bool isForeground = false;
        for (auto f : foregrounds) {
            if (f == b) { isForeground = true; break; }
        }
        if (!isForeground) b->Render();
    }
    for (auto i : items) i->Render();
    for (auto e : enemies) e->Render();
    mario->Render();
    for (auto f : foregrounds) f->Render();
}

void CPlayScene::Unload() {
    delete mario;
    mario = nullptr;

    // Delete blocks, but skip bridges (they're owned by foregrounds)
    for (auto b : blocks) {
        // Check if this block is also in foregrounds
        bool inForegrounds = false;
        for (auto f : foregrounds) {
            if (f == b) { inForegrounds = true; break; }
        }
        if (!inForegrounds) delete b;
    }
    blocks.clear();

    for (auto d : decors) delete d;
    decors.clear();

    for (auto e : enemies) delete e;
    enemies.clear();

    for (auto i : items) delete i;
    items.clear();

    for (auto f : foregrounds) delete f;
    foregrounds.clear();
}
