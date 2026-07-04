#pragma once
#include "../CGameObject.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/core/Game.h"
#include "../../registry/CResourceRegistry.h"
#include "../player/CMario.h"

enum class EFireballState
{
    WAITING,
    ACTIVE
};

class CBowserFireball : public CGameObject
{
    EFireballState state;

public:
    CBowserFireball(float x, float y) : CGameObject(x, y)
    {
        this->type = "bowser_fireball";
        this->state = EFireballState::WAITING;
        this->vx = 0.0f;
        this->vy = 0.0f;
    }
    void Update(float dt) override
    {
        if (state == EFireballState::WAITING)
        {
            float cx = CCamera::GetInstance()->GetCamX();
            float sw = (float)CGame::GetInstance()->GetViewportWidth();
            // Tự kích hoạt khi biên bên phải của camera bao trọn vị trí quả cầu lửa (lại gần)
            if (cx + sw >= x)
            {
                state = EFireballState::ACTIVE;
                vx = -0.06f; // Di chuyển ngang đều sang bên trái
            }
        }
        else if (state == EFireballState::ACTIVE)
        {
            x += vx * dt;
            // Tự hủy nếu đạn lửa bay ra khỏi phía bên trái màn hình
            if (x < CCamera::GetInstance()->GetCamX() - 32.0f)
            {
                SetIsDead(true);
            }
        }
    }
    void Render() override
    {
        if (state == EFireballState::ACTIVE)
        {
            CResourceRegistry::GetInstance()->RenderAnimation("ANI_BOWSER_FIREBALL", x, y, -1);
        }
    }
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override
    {
        if (state == EFireballState::WAITING || IsDead())
        {
            left = bottom = right = top = 0.0f;
            return;
        }
        left = x;
        bottom = y;
        right = x + 24.0f;
        top = y + 8.0f;
    }
    bool IsSolid() override { return false; }
    bool IsBlocking(CGameObject *other) override { return false; }
    void OnCollision(CGameObject *other) override
    {
        if (auto mario = dynamic_cast<CMario *>(other))
        {
            mario->Hurt(); // Gây sát thương cho Mario khi va chạm
        }
    }
};