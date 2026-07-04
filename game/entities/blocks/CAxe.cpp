#include "CAxe.h"
#include "../../scenes/play/CPlayScene.h"
#include "CBridge.h"
#include "CDecorBlock.h"
#include "../enemies/CBowser.h"
#include "../../../engine/audio/CAudioManager.h"
#include "../../../engine/core/Game.h"
#include "../../registry/CResourceRegistry.h"
#include <algorithm>

void CAxe::OnCollision(CGameObject *obj)
{
    if (auto mario = dynamic_cast<CMario *>(obj))
    {
        // 1. Tắt nhạc nền lâu đài và phát nhạc thắng màn world_clear.wav
        CAudioManager::GetInstance()->StopBGM();
        CAudioManager::GetInstance()->PlayBGM("content/audio/world_clear.wav");

        // 2. Kích hoạt sập cầu bằng cách thu thập và sắp xếp các block CCastleBridge
        scene->castleClearState = ECastleClearState::BRIDGE_COLLAPSING;
        scene->castleClearTimer = 0.0f;
        scene->bridgeBlocks.clear();

        for (auto b : scene->blocks)
        {
            if (auto bridge = dynamic_cast<CBridge *>(b))
            {
                // Chỉ lấy cầu sắt lâu đài (ANI_WHITE_RED_STEEL_BRIDGE)
                if (bridge->GetAniName() == "ANI_WHITE_RED_STEEL_BRIDGE")
                    scene->bridgeBlocks.push_back(bridge);
            }
        }

        // Sắp xếp các block cầu từ trái qua phải (theo trục x tăng dần)
        std::sort(scene->bridgeBlocks.begin(), scene->bridgeBlocks.end(), [](CBridge *a, CBridge *b)
                  { return a->x < b->x; });
        scene->nextBridgeBlockIndex = 0;

        // 2b. Đứt xích (ANI_CHAIN)
        for (auto d : scene->decors)
        {
            if (auto decor = dynamic_cast<CDecorBlock *>(d))
            {
                if (decor->GetAniName() == "ANI_CHAIN")
                {
                    decor->SetIsDead(true);
                }
            }
        }

        // 3. Khóa phím điều khiển của Mario và đặt tốc độ về 0
        mario->SetInputLocked(true);
        mario->vx = 0.0f;
        mario->vy = 0.0f;

        // 4. Ẩn rìu
        this->SetIsDead(true);
    }
}

void CAxe::Render()
{
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_AXE", x, y);
}

void CAxe::GetBoundingBox(float &left, float &bottom, float &right, float &top)
{
    left = x;
    bottom = y;
    right = x + 16.0f;
    top = y + 16.0f;
}