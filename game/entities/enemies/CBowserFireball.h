#pragma once
#include "../CGameObject.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/core/Game.h"
#include "../../registry/CResourceRegistry.h"
#include "../player/CMario.h"
#include "../blocks/CBlock.h"
#include "../blocks/CBridge.h"

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

            float cx = CCamera::GetInstance()->GetCamX();
            float sw = (float)CGame::GetInstance()->GetViewportWidth();
            // Tự hủy nếu bay ra khỏi khung hình camera bên trái hoặc bên phải
            if (x < cx - 32.0f || x > cx + sw + 32.0f)
            {
                SetIsDead(true);
            }
        }
    }
    void Render() override
    {
        if (state == EFireballState::ACTIVE)
        {
            CResourceRegistry::GetInstance()->RenderAnimation("ANI_BOWSER_FIREBALL", x, y, 1);
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
    bool IsBlocking(CGameObject *other) override
    {
        // Chỉ va chạm với Mario và các khối địa hình/gạch.
        // Tránh va chạm với Bowser hay các quái khác khiến đạn bị chặn và bật ngược lại.
        if (dynamic_cast<CMario *>(other) || dynamic_cast<CBlock *>(other))
            return true;
        return false;
    }
    void OnCollision(CGameObject *other) override
    {
        if (auto mario = dynamic_cast<CMario *>(other))
        {
            mario->Hurt(); // Gây sát thương cho Mario khi va chạm
        }
    }
    void OnCollisionX(CGameObject *other, float nx) override
    {
        if (dynamic_cast<CBlock *>(other) && !dynamic_cast<CBridge *>(other))
        {
            SetIsDead(true); // Biến mất khi chạm tường/ống
        }
    }
    void OnCollisionY(CGameObject *other, float ny) override
    {
        if (dynamic_cast<CBlock *>(other) && !dynamic_cast<CBridge *>(other))
        {
            SetIsDead(true); // Biến mất khi chạm trần/sàn
        }
    }
};