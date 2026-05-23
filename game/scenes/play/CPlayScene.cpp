#include "CPlayScene.h"
#include "../../../engine/core/Game.h"
#include "../../../engine/input/KeyboardManager.h"
#include "../../../engine/physics/CCollision.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/utils/debug.h"
#include "../../../engine/utils/CScoreManager.h"
#include "../../entities/blocks/CPipe.h"
#include "../../registry/CMapLoader.h"
#include "../../registry/CResourceRegistry.h"
#include <algorithm>

void CPlayScene::Load()
{
    Load(levelPath);
}

void CPlayScene::Load(const std::string& mapPath)
{
    auto registry = CResourceRegistry::GetInstance();
    registry->LoadResourcesForPlayScene();

    currentMapPath = mapPath;
    pendingMapPath = "";

    mario = nullptr; // MapLoader sẽ khởi tạo Mario
    CMapLoader::GetInstance()->Load(mapPath, this);

    if (mario == nullptr)
    {
        DebugOut(L"[WARNING] Mario not found, starting manual initialization.\n");
        mario = new CMario();
        mario->x = 100.0f;
        mario->y = 150.0f;
    }

    CCamera::GetInstance()->SetCamPos(0, 0);
    CCamera::GetInstance()->SetMapWidth(0); // Will be set in process map or update

    DebugOut(L"[INFO] CPlayScene::Load completed. Map: %hs, Blocks: %d, Decors: %d\n",
             mapPath.c_str(), (int)blocks.size(), (int)decors.size());
}

void CPlayScene::Update(float dt)
{
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

    // Lọc các blocks, enemies và items ở trong khoảng 256 pixels quanh Mario để
    // tối ưu collision checks
    auto nearbyBlocks = GetObjectsInRange(mario->x, mario->y, blocks);
    auto nearbyEnemies = GetObjectsInRange(mario->x, mario->y, enemies);
    auto nearbyItems = GetObjectsInRange(mario->x, mario->y, items);

    std::vector<CGameObject*> coObjectsForMario;
    coObjectsForMario.reserve(nearbyBlocks.size() + nearbyEnemies.size() + nearbyItems.size());
    coObjectsForMario.insert(coObjectsForMario.end(), nearbyBlocks.begin(), nearbyBlocks.end());
    coObjectsForMario.insert(coObjectsForMario.end(), nearbyEnemies.begin(), nearbyEnemies.end());
    coObjectsForMario.insert(coObjectsForMario.end(), nearbyItems.begin(), nearbyItems.end());

    if (mario->GetState() != EMarioState::DIE) {
        CCollision::ResolveCollision(mario, dt, coObjectsForMario);
    }

    // Xử lý collision cho từng enemy với block xung quanh nó
    for (auto e : enemies)
    {
        auto blocksAroundEnemy = GetObjectsInRange(e->x, e->y, blocks);
        CCollision::ResolveCollision(e, dt, blocksAroundEnemy);
    }

    // Xử lý collision cho từng item với block xung quanh nó
    for (auto i : items)
    {
        auto blocksAroundItem = GetObjectsInRange(i->x, i->y, blocks);
        CCollision::ResolveCollision(i, dt, blocksAroundItem);
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

    // Logic đi xuống ống nước
    auto kb = KeyboardManager::GetInstance();
    if (kb->IsKeyPressed('S') || kb->IsKeyPressed(VK_DOWN))
        for (auto b : blocks)
            if (auto pipe = dynamic_cast<CPipe*>(b))
                if (pipe->IsWarpPipe() && pipe->GetEnterDirection() == "down")
                {
                    float mLeft, mBottom, mRight, mTop;
                    mario->GetBoundingBox(mLeft, mBottom, mRight, mTop);
                    float pLeft, pBottom, pRight, pTop;
                    pipe->GetBoundingBox(pLeft, pBottom, pRight, pTop);

                    // Chân của Mario phải cách đỉnh ống nước trong khoảng 2 pixel
                    if (mBottom >= pTop - 2.0f && mBottom <= pTop + 2.0f &&
                        mLeft < pRight && mRight > pLeft)
                    {
                        DebugOut(L"[INFO] Entering pipe to map: %hs\n",
                                 pipe->GetDestMap().c_str());
                        std::string dest = "content/levels/" + pipe->GetDestMap() + ".csv";
                        TransitionToMap(dest);
                        break;
                    }
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

    // Chuyển map
    if (!pendingMapPath.empty())
    {
        std::string nextMap = pendingMapPath;
        Unload();
        Load(nextMap);
    }
}

void CPlayScene::Render()
{
    ID3DX10Sprite* spriteHandler = CGame::GetInstance()->GetSpriteHandler();

    // Layer 1: Background (hills, clouds, bushes, flag, castle)
    for (auto d : decors)
        d->Render();
    // Đẩy lên GPU batch chứa riêng decors
    spriteHandler->Flush();

    // Layer 2: blocks, items, enemies, mario (skip foregrounds in this pass)
    for (auto b : blocks) {
        // Skip bridges/foregrounds in normal pass - they render in foreground layer
        bool isForeground = false;
        for (auto f : foregrounds) {
            if (f == b) { isForeground = true; break; }
        }
        if (!isForeground) b->Render();
    }
    for (auto i : items) i->Render();
    for (auto e : enemies) e->Render();
    mario->Render();

    // Layer 3: Foreground (bridges render over Mario)
    for (auto f : foregrounds) f->Render();
}

void CPlayScene::Unload()
{
    delete mario;
    mario = nullptr;

    // Delete blocks, but skip bridges (they're owned by foregrounds)
    for (auto b : blocks) {
        bool inForegrounds = false;
        for (auto f : foregrounds) {
            if (f == b) { inForegrounds = true; break; }
        }
        if (!inForegrounds) delete b;
    }
    blocks.clear();

    for (auto d : decors)
        delete d;
    decors.clear();

    for (auto e : enemies)
        delete e;
    enemies.clear();

    for (auto i : items)
        delete i;
    items.clear();

    for (auto f : foregrounds) delete f;
    foregrounds.clear();
}
