#include "CAxe.h"
#include "../../scenes/play/CPlayScene.h"
#include "CCastleBridge.h"
#include "../../../engine/audio/CAudioManager.h"
#include "../../../engine/core/Game.h"
#include "../../registry/CResourceRegistry.h"

void CAxe::OnCollision(CGameObject *obj)
{
    if (dynamic_cast<CMario *>(obj))
    {
        // 1. Tắt nhạc nền lâu đài và phát nhạc thắng màn
        CAudioManager::GetInstance()->StopBGM();
        CAudioManager::GetInstance()->Play("stage_clear");

        // 2. Kích hoạt sập cầu bằng cách đánh dấu xóa các block CCastleBridge
        for (auto b : scene->blocks)
        {
            if (auto bridge = dynamic_cast<CCastleBridge *>(b))
            {
                bridge->SetIsDead(true); // Đánh dấu chết để CPlayScene::Update xóa
            }
        }

        // 3. Ẩn rìu
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