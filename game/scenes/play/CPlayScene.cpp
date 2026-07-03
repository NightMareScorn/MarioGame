#include "CPlayScene.h"
#include "../../../engine/core/Game.h"
#include "../../../engine/input/KeyboardManager.h"
#include "../../../engine/physics/CCollision.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/utils/debug.h"
#include "../../../engine/utils/CScoreManager.h"
#include "../../../engine/audio/CAudioManager.h"
#include "../../entities/blocks/CPipe.h"
#include "../../entities/items/CFireball.h"
#include "../../entities/items/CFireFlower.h"
#include "../../entities/items/CMushroom.h"
#include "../../entities/blocks/CFlagpole.h"
#include "../../registry/CMapLoader.h"
#include "../../registry/CResourceRegistry.h"
#include "../../../engine/rendering/Textures.h"
#include <algorithm>

void CPlayScene::Load()
{
    Load(levelPath);
}

void CPlayScene::Load(const std::string &mapPath)
{
    scene = this; // Gán con trỏ scene toàn cục
    auto registry = CResourceRegistry::GetInstance();
    registry->LoadResourcesForPlayScene();

    currentMapPath = mapPath;
    pendingMapPath = "";
    goalTimer = 0.0f;

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
    CCamera::GetInstance()->SetMapWidth(0);

    DebugOut(L"[INFO] CPlayScene::Load completed. Map: %hs, Blocks: %d, Decors: %d\n",
             mapPath.c_str(), (int)blocks.size(), (int)decors.size());

    CAudioManager::GetInstance()->PlayBGM("content/audio/overworld_theme.wav");
}

void CPlayScene::Update(float dt)
{
    if (isGameOver)
    {
        if (KeyboardManager::GetInstance()->IsKeyPressed(VK_RETURN))
        {
            CMario::lives = 3;
            CMario::hasCheckpoint = false;
            CMario::currentPower = EMarioPower::SMALL;
            CGame::GetInstance()->SetExitLevel(true);
        }
        return;
    }

    auto kb = KeyboardManager::GetInstance();

    if (kb->IsKeyPressed('P') || kb->IsKeyPressed('p'))
    {
        isPaused = !isPaused;
        if (isPaused)
        {
            mciSendStringA("pause bgm", NULL, 0, NULL); // Tạm dừng nhạc nền
        }
        else
        {
            mciSendStringA("resume bgm", NULL, 0, NULL); // Phát tiếp nhạc nền
        }
    }

    if (isPaused)
    {
        if (kb->IsKeyPressed(VK_UP))
        {
            pauseSelection = 0; // Chọn Mute
        }
        else if (kb->IsKeyPressed(VK_DOWN))
        {
            pauseSelection = 1; // Chọn Back to Menu
        }
        if (kb->IsKeyPressed(VK_RETURN))
        {
            if (pauseSelection == 0)
            {
                // Logic bật tắt âm thanh (Mute/Unmute)
                static bool isMuted = false;
                isMuted = !isMuted;
                if (isMuted)
                {
                    mciSendStringA("setaudio bgm volume to 0", NULL, 0, NULL);
                }
                else
                {
                    mciSendStringA("setaudio bgm volume to 1000", NULL, 0, NULL);
                }
            }
            else if (pauseSelection == 1)
            {
                isPaused = false;
                CGame::GetInstance()->SetExitLevel(true); // Thoát về Menu WebView2
            }
        }
        return; // Dừng toàn bộ cập nhật vật lý, quái vật, thời gian bên dưới!
    }

    // Chuyển map pending
    if (!pendingMapPath.empty())
    {
        std::string nextMap = pendingMapPath;
        Unload();
        Load(nextMap);
        return;
    }

    if (!mario)
        return;

    // --- LOGIC HỒI SINH KHI CHẾT ---
    if (mario->IsDeadState())
    {
        mario->Update(dt);
        if (mario->GetDieTimer() <= 0)
        {
            if (CMario::lives > 0)
            {
                this->TransitionToMap(currentMapPath);
            }
            else
            {
                isGameOver = true;
                CAudioManager::GetInstance()->StopBGM();
            }
        }
        return;
    }

    // --- LOGIC THẮNG MÀN (CỘT CỜ) ---
    if (goalTimer > 0)
    {
        goalTimer -= dt;
        for (auto d : decors)
            d->Update(dt);

        if (mario->GetState() == EMarioState::GOAL_SLIDE)
        {
            if (mario->y <= 32.5f)
            { // Chạm đất
                mario->y = 32.0f;
                mario->SetState(static_cast<int>(EMarioState::GOAL_WALK));
            }
        }
        mario->Update(dt);
        mario->SetOnGround(true);
        CCamera::GetInstance()->Update(mario->x, mario->y, (DWORD)dt);

        if (goalTimer <= 0)
        {
            CMario::hasCheckpoint = false;
            CMario::currentPower = EMarioPower::SMALL;
            // Chuyển sang màn tiếp theo
            std::string nextLevel = "content/levels/level_1_1.csv";

            bool shouldExitToMenu = false;
            if (levelPath.find("level_1_1.csv") != std::string::npos)
            {
                nextLevel = "content/levels/level_1_2_first_half.csv";
            }
            else if (levelPath.find("level_1_2_second_half.csv") != std::string::npos)
            {
                nextLevel = "content/levels/level_1_3.csv";
            }
            else if (levelPath.find("level_1_3.csv") != std::string::npos)
            {
                nextLevel = "content/levels/level_1_4.csv";
            }

            // Thiếu 2 màn chơi 2.1 và 2.2
            else if (levelPath.find("level_2_3.csv") != std::string::npos)
            {
                nextLevel = "content/levels/level_2_4.csv";
            }
            else
            {
                shouldExitToMenu = true;
            }

            if (shouldExitToMenu)
            {
                CGame::GetInstance()->SetExitLevel(true);
            }
            else
            {
                this->TransitionToMap(nextLevel);
            }
        }
        return;
    }

    // 1. HUD logic update (timeLeft)
    if (timeLeft > 0)
    {
        timeLeft -= (dt / 1000.0f);
        if (timeLeft < 0)
            timeLeft = 0;
    }

    // Update các entities
    for (auto b : blocks)
        b->Update(dt);
    for (auto e : enemies)
        e->Update(dt);
    for (auto i : items)
        i->Update(dt);
    for (auto d : decors)
        d->Update(dt);

    // Sync Mario with moving platforms
    for (auto b : blocks)
    {
        if (b->type == "platform")
        {
            float ml, mt, mr, mb;
            mario->GetBoundingBox(ml, mt, mr, mb);
            float pl, pt, pr, pb;
            b->GetBoundingBox(pl, pt, pr, pb);

            if (mr > pl && ml < pr && mb >= pb && mb <= pb + 4.0f && mario->vy <= 0)
            {
                mario->x += b->vx * dt;
                mario->y += b->vy * dt;

                if (b->vy < 0)
                {
                    mario->y = pb + 0.1f;
                    mario->SetOnGround(true);
                }
            }
        }
    }

    mario->Update(dt);

    // Lọc đối tượng xung quanh Mario để tối ưu va chạm
    auto nearbyBlocks = GetObjectsInRange(mario->x, mario->y, blocks);
    auto nearbyEnemies = GetObjectsInRange(mario->x, mario->y, enemies);
    auto nearbyItems = GetObjectsInRange(mario->x, mario->y, items);

    std::vector<CGameObject *> coObjectsForMario;
    coObjectsForMario.reserve(nearbyBlocks.size() + nearbyEnemies.size() + nearbyItems.size());
    coObjectsForMario.insert(coObjectsForMario.end(), nearbyBlocks.begin(), nearbyBlocks.end());
    coObjectsForMario.insert(coObjectsForMario.end(), nearbyEnemies.begin(), nearbyEnemies.end());
    coObjectsForMario.insert(coObjectsForMario.end(), nearbyItems.begin(), nearbyItems.end());

    CCollision::ResolveCollision(mario, dt, coObjectsForMario);

    // --- LOGIC STAR MODE: Chạm vào quái là quái chết ---
    if (mario->IsStarMode())
    {
        float ml, mb, mr, mt;
        mario->GetBoundingBox(ml, mb, mr, mt);
        for (auto e : nearbyEnemies)
        {
            if (e->IsDead())
                continue;
            float el, eb, er, et;
            e->GetBoundingBox(el, eb, er, et);
            if (CCollision::CheckAABB({ml, mb, mr, mt}, {el, eb, er, et}))
            {
                e->OnStomped();
                this->AddScore(100);
            }
        }
    }

    // --- LOGIC FLAGPOLE (CỘT CỜ) ---
    if (!mario->IsInputLocked())
    {
        float ml, mb, mr, mt;
        mario->GetBoundingBox(ml, mb, mr, mt);
        for (auto d : decors)
        {
            if (auto flagpole = dynamic_cast<CFlagpole *>(d))
            {
                float dl, db, dr, dt_b;
                flagpole->GetBoundingBox(dl, db, dr, dt_b);
                if (CCollision::CheckAABB({ml, mb, mr, mt}, {dl, db, dr, dt_b}))
                {
                    mario->x = dl - 4.0f;
                    mario->HitGoal();
                    flagpole->SetState(200); // Hạ cờ
                    goalTimer = 4000.0f;     // 4 giây trượt cờ và chạy vào lâu đài
                    break;
                }
            }
        }
    }

    // Xử lý collision cho từng enemy
    for (auto e : enemies)
    {
        if (e->IsDead())
            continue;
        float old_vx = e->vx;

        auto blocksAroundEnemy = GetObjectsInRange(e->x, e->y, blocks);
        auto enemiesAroundEnemy = GetObjectsInRange(e->x, e->y, enemies);

        auto it = std::find(enemiesAroundEnemy.begin(), enemiesAroundEnemy.end(), e);
        if (it != enemiesAroundEnemy.end())
            enemiesAroundEnemy.erase(it);

        std::vector<CGameObject *> coObjectsForEnemy = blocksAroundEnemy;
        coObjectsForEnemy.insert(coObjectsForEnemy.end(), enemiesAroundEnemy.begin(), enemiesAroundEnemy.end());

        CCollision::ResolveCollision(e, dt, coObjectsForEnemy);

        // Quay đầu nếu đụng tường
        if (e->vx == 0 && old_vx != 0)
        {
            e->vx = -old_vx;
            e->nx = (e->vx > 0) ? 1 : -1;
        }

        // Logic tuần tra: tránh rơi xuống vực thẳm
        if (e->vy == 0 && e->vx != 0)
        {
            float L, B, R, T;
            e->GetBoundingBox(L, B, R, T);

            CCollision::Box sensor;
            if (e->vx < 0)
            {
                sensor = CCollision::ToBox(L, B - 4.0f, L + 2.0f, B - 0.5f);
            }
            else
            {
                sensor = CCollision::ToBox(R - 2.0f, B - 4.0f, R, B - 0.5f);
            }

            bool hasFloor = false;
            for (auto b : blocksAroundEnemy)
            {
                float bl, bb, br, bt;
                b->GetBoundingBox(bl, bb, br, bt);
                if (CCollision::CheckAABB(sensor, CCollision::ToBox(bl, bb, br, bt)))
                {
                    hasFloor = true;
                    break;
                }
            }

            if (!hasFloor)
            {
                e->vx = -e->vx;
                e->nx = (e->vx > 0) ? 1 : -1;
            }
        }
    }

    // Xử lý collision cho từng item
    for (auto i : items)
    {
        if (i->IsDead())
            continue;
        if (!dynamic_cast<CFireFlower *>(i) && !dynamic_cast<CFireball *>(i))
        {
            auto blocksAroundItem = GetObjectsInRange(i->x, i->y, blocks);
            CCollision::ResolveCollision(i, dt, blocksAroundItem);
        }
    }

    mario->UpdateState();

    if (mapWidth > 0 && mario->x > mapWidth - 16)
        mario->x = mapWidth - 16;
    mario->SetMapWidth(mapWidth);

    if (mario->y < -64.0f)
    {
        mario->Die();
        return;
    }

    // Logic đi xuống ống nước
    if (kb->IsKeyPressed('S') || kb->IsKeyPressed(VK_DOWN))
        for (auto b : blocks)
            if (auto pipe = dynamic_cast<CPipe *>(b))
                if (pipe->IsWarpPipe() && pipe->GetEnterDirection() == "down")
                {
                    float mLeft, mBottom, mRight, mTop;
                    mario->GetBoundingBox(mLeft, mBottom, mRight, mTop);
                    float pLeft, pBottom, pRight, pTop;
                    pipe->GetBoundingBox(pLeft, pBottom, pRight, pTop);

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

    // Dọn dẹp items & enemies bị chết
    items.erase(std::remove_if(items.begin(), items.end(), [](CGameObject *o)
                               { 
        if (o->IsDead()) { delete o; return true; } 
        return false; }),
                items.end());

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](CGameObject *o)
                                 { 
        if (o->IsDead()) { delete o; return true; } 
        return false; }),
                  enemies.end());

    // Dọn dẹp blocks bị chết (ví dụ: gạch cầu bị sập)
    blocks.erase(std::remove_if(blocks.begin(), blocks.end(), [](CBlock *o)
                                { 
        if (o->IsDead()) { delete o; return true; } 
        return false; }),
                 blocks.end());
}

void CPlayScene::Render()
{
    if (isGameOver)
    {
        CGame *g = CGame::GetInstance();

        auto dummyTex = CTextures::GetInstance()->Get(2);
        g->Draw(0, 0, dummyTex, 0, 0, 1, 1, 1.0f, g->GetViewportWidth(), g->GetViewportHeight());
        // Game Over text
        RECT rectGameOver = {0, 200, g->GetBackBufferWidth(), 260};
        g->DrawTextRaw(L"GAME OVER", rectGameOver, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
        // Enter to return to menu text
        RECT rectHelp = {0, 300, g->GetBackBufferWidth(), 340};
        g->DrawTextRaw(L"Press Enter to Back to Main Menu", rectHelp, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
        return;
    }

    ID3DX10Sprite *spriteHandler = CGame::GetInstance()->GetSpriteHandler();

    // Layer 1: Background
    for (auto d : decors)
        d->Render();
    spriteHandler->Flush();

    // Layer 2: blocks, items, enemies, mario
    for (auto b : blocks)
    {
        bool isForeground = false;
        for (auto f : foregrounds)
        {
            if (f == b)
            {
                isForeground = true;
                break;
            }
        }
        if (!isForeground)
            b->Render();
    }
    for (auto i : items)
        if (!i->IsDead())
            i->Render();
    for (auto e : enemies)
        if (!e->IsDead())
            e->Render();
    if (mario)
        mario->Render();

    // Layer 3: Foreground
    for (auto f : foregrounds)
        f->Render();

    if (isPaused)
    {
        CGame *g = CGame::GetInstance();

        auto dummyTex = CTextures::GetInstance()->Get(2);
        g->Draw(0, 0, dummyTex, 0, 0, 1, 1, 0.5f, g->GetViewportWidth(), g->GetViewportHeight());

        RECT rectPause = {0, 80, g->GetBackBufferWidth(), 120};
        g->DrawTextRaw(L"GAME PAUSED", rectPause, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
        RECT rectMute = {0, 160, g->GetBackBufferWidth(), 190};
        g->DrawTextRaw(pauseSelection == 0 ? L"> MUTE GAME <" : L"  MUTE GAME  ", rectMute, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
        RECT rectMenu = {0, 220, g->GetBackBufferWidth(), 250};
        g->DrawTextRaw(pauseSelection == 1 ? L"> BACK TO MAIN MENU <" : L"  BACK TO MAIN MENU  ", rectMenu, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
    }
}

void CPlayScene::Unload()
{
    delete mario;
    mario = nullptr;

    for (auto b : blocks)
    {
        bool inForegrounds = false;
        for (auto f : foregrounds)
        {
            if (f == b)
            {
                inForegrounds = true;
                break;
            }
        }
        if (!inForegrounds)
            delete b;
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

    for (auto f : foregrounds)
        delete f;
    foregrounds.clear();

    CAudioManager::GetInstance()->StopBGM();
}
