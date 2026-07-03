#include "CAxe.h"
#include "../../scenes/play/CPlayScene.h"
#include "CCastleBridge.h"
#include "../../../engine/audio/CAudioManager.h"
#include "../../../engine/core/Game.h"
#include "../../registry/CResourceRegistry.h"

void CAxe::OnCollision(CGameObject *obj)
{
    if (auto mario = dynamic_cast<CMario *>(obj))
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

        // 3. Khóa phím điều khiển của Mario và đặt tốc độ về 0
        mario->SetInputLocked(true);
        mario->vx = 0.0f;
        mario->vy = 0.0f;

        // 4. Kích hoạt đếm ngược thắng màn để tự động chuyển tiếp sang World tiếp theo
        scene->goalTimer = 6000.0f;

        // 5. Ẩn rìu
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