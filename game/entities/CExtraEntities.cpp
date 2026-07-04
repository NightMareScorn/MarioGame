#include "enemies/CBowser.h"
#include "enemies/CFireBar.h"
#include "enemies/CPodoboo.h"
#include "blocks/CPlatform.h"
#include "blocks/CCastleBridge.h"
#include "player/CMario.h"
#include "../registry/CResourceRegistry.h"
#include "items/CFireball.h"
#include "../scenes/play/CPlayScene.h"
#include "../scenes/CSceneManager.h"
#include "enemies/CBowserFireball.h"

#include <cmath>

// --- Render Implementations ---
void CBowser::Render()
{
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_BOWSER_WALK", x, y, -nx);
}

void CFireBar::Render()
{
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_FIRE_BAR_ORB", x, y);
}

void CPodoboo::Render()
{
    if (state == JUMPING)
    {
        CResourceRegistry::GetInstance()->RenderAnimation("ANI_FIRE_LAVA_SHOOT", x, y);
    }
}

void CPlatform::Render()
{
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_MOVING_PLATFORM", x, y);
}

void CCastleBridge::Render()
{
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_BRIDGE_BLOCK", x, y);
}

void CBowser::Update(float dt)
{
    CPlayScene *playScene = (CPlayScene *)CSceneManager::GetInstance()->GetCurrentScene();
    if (playScene && playScene->GetCastleClearState() != ECastleClearState::NONE)
    {
        if (playScene->GetCastleClearState() == ECastleClearState::BRIDGE_COLLAPSING)
        {
            vx = 0.0f;
            vy = 0.0f;
            return;
        }
        else if (playScene->GetCastleClearState() == ECastleClearState::BOWSER_FALLING)
        {
            vx = 0.0f;
            vy += -0.0005f * dt;
            x += vx * dt;
            y += vy * dt;
            return;
        }
        else
        {
            vx = 0.0f;
            vy = 0.0f;
            return;
        }
    }

    // Áp dụng trọng lực nhẹ cho Bowser
    vy += -0.0005f * dt;

    // Hướng khuôn mặt Bowser luôn nhìn về phía Mario
    CMario *mario = playScene ? playScene->GetPlayer() : nullptr;
    if (mario)
    {
        if (mario->x < x)
            nx = -1; // Mặt hướng qua trái
        else
            nx = 1;  // Mặt hướng qua phải
    }

    // Di chuyển qua lại quanh startX
    x += vx * dt;
    if (abs(x - startX) > 64.0f)
    {
        vx = -vx;
    }

    // Cơ chế nhảy ngẫu nhiên từ 1-3 giây
    jumpTimer += dt;
    if (jumpTimer >= nextJumpTime)
    {
        jumpTimer = 0.0f;
        nextJumpTime = 1000.0f + (float)(rand() % 2001); // Tính thời gian nhảy kế tiếp
        if (isOnGround)
        {
            vy = 0.16f; // Vận tốc nhảy lên
            isOnGround = false;
        }
    }

    // Cự ly bắn đạn lửa tự động
    fireTimer += dt;
    if (fireTimer > 3000.0f)
    {
        fireTimer = 0;
        // Bắn ra đạn lửa bay theo đúng hướng khuôn mặt của Bowser
        CBowserFireball *fire = new CBowserFireball(this->x + (nx < 0 ? -16.0f : 32.0f), this->y + 8.0f);
        fire->vx = 0.08f * (float)nx; // Vận tốc đạn bay theo nx
        fire->SetState(1); // ACTIVE
        scene->AddEnemy(fire);
    }
}

void CBowser::GetBoundingBox(float &left, float &bottom, float &right, float &top)
{
    left = x;
    bottom = y;
    right = x + 32;
    top = y + 32;
}

void CFireBar::Update(float dt)
{
    angle += rotationSpeed * dt;
    x = centerX + offset * cos(angle);
    y = centerY + offset * sin(angle);
}
void CFireBar::GetBoundingBox(float &left, float &bottom, float &right, float &top)
{
    left = x;
    bottom = y;
    right = x + 8;
    top = y + 8;
}
void CFireBar::OnCollision(CGameObject *other)
{
    if (auto mario = dynamic_cast<CMario *>(other))
    {
        mario->StartDeath();
    }
}

void CPodoboo::Update(float dt)
{
    if (state == WAITING)
    {
        vy += dt; // Reuse vy as timer
        if (vy > 1500)
        { // Wait 1.5s
            state = JUMPING;
            vy = 0.32f;
        }
    }
    else
    {
        y += vy * dt;
        vy -= 0.0004f * dt;
        if (y < startY)
        {
            y = startY;
            state = WAITING;
            vy = 0;
        }
    }
}
void CPodoboo::GetBoundingBox(float &left, float &bottom, float &right, float &top)
{
    left = x;
    bottom = y;
    right = x + 12;
    top = y + 14;
}
void CPodoboo::OnCollision(CGameObject *other)
{
    if (auto mario = dynamic_cast<CMario *>(other))
    {
        mario->StartDeath();
    }
}

void CPlatform::Update(float dt)
{
    if (isVertical)
    {
        vx = 0;
        vy = speed;
        y += vy * dt;
        // Wrap around NES height (240px)
        if (speed > 0 && y > 240)
            y = -16;
        else if (speed < 0 && y < -16)
            y = 240;
    }
    else
    {
        vy = 0;
        vx = speed;
        x += vx * dt;
        // Horizontal platforms in 2-4 usually bounce between some limits
        // For simplicity, let's just make it bounce within a 48px range
        if (startX == -1.0f)
            startX = x;
        if (abs(x - startX) > 48)
        {
            speed = -speed;
            x = (x > startX) ? startX + 48 : startX - 48;
        }
    }
}
void CPlatform::GetBoundingBox(float &left, float &bottom, float &right, float &top)
{
    left = x;
    bottom = y;
    right = x + 24;
    top = y + 8;
}

void CCastleBridge::Update(float dt) {}
void CCastleBridge::GetBoundingBox(float &left, float &bottom, float &right, float &top)
{
    left = x;
    bottom = y;
    right = x + 16;
    top = y + 16;
}

void CBowser::OnCollisionX(CGameObject *other, float nx)
{
    if (auto mario = dynamic_cast<CMario *>(other))
    {
        mario->Hurt();
    }
}

void CBowser::OnCollisionY(CGameObject *other, float ny)
{
    if (ny < 0)
    {
        isOnGround = true; // Chạm đất
        vy = 0;
    }
    if (auto mario = dynamic_cast<CMario *>(other))
    {
        mario->Hurt();
    }
}