#include "CKoopa.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../entities/player/CMario.h"
#include "CGoomba.h"
#include "../../../engine/audio/CAudioManager.h"

CKoopa::CKoopa(float x, float y, std::string type, float patrolLeft, float patrolRight)
    : CGameObject(), type(type)
{
    this->x = x;
    this->y = y;
    this->patrolLeft = patrolLeft;
    this->patrolRight = patrolRight;
    SetState(KOOPA_STATE_WALKING);
    this->nx = -1;
}

void CKoopa::SetState(int s)
{
    state = s;
    if (state == KOOPA_STATE_WALKING)
    {
        vx = -0.03f;
    }
    else if (state == KOOPA_STATE_SHELL)
    {
        vx = 0;
        vy = 0;
    }
    else if (state == KOOPA_STATE_SHELL_RUNNING)
    {
        vx = 0.15f * nx;
    }
}

void CKoopa::Update(float dt)
{
    vy -= 0.002f * dt;

    if (this->state == KOOPA_STATE_WALKING)
    {
        if ((patrolLeft != 0 || patrolRight != 0) && (patrolLeft < patrolRight))
        {
            if (x < patrolLeft && vx < 0)
            {
                x = patrolLeft;
                vx = -vx;
                nx = 1;
            }
            else if (x > patrolRight && vx > 0)
            {
                x = patrolRight;
                vx = -vx;
                nx = -1;
            }
        }
    }
}

void CKoopa::Render()
{
    std::string ani = "ANI_KOOPA_WALK";
    if (state == KOOPA_STATE_SHELL || state == KOOPA_STATE_SHELL_RUNNING)
    {
        ani = "ANI_KOOPA_SHELL";
    }
    CAnimations::GetInstance()->Render(ani, x, y, -nx);
}

void CKoopa::GetBoundingBox(float &left, float &bottom, float &right, float &top)
{
    left = x;
    bottom = y;
    right = x + 16.0f;
    if (state == KOOPA_STATE_WALKING)
    {
        top = y + 24.0f;
    }
    else
    {
        top = y + 16.0f;
    }
}

#include "../../scenes/play/CPlayScene.h"

void CKoopa::OnCollisionX(CGameObject *other, float nx)
{
    if (auto mario = dynamic_cast<CMario *>(other))
    {
        float ml, mb, mr, mt;
        mario->GetBoundingBox(ml, mb, mr, mt);
        float kl, kb, kr, kt;
        this->GetBoundingBox(kl, kb, kr, kt);

        // Nếu Mario đang rơi xuống từ phía trên, không làm Mario bị thương ở trục X (chờ trục Y xử lý dẫm)
        if (mario->vy <= 0 && mb > kb)
            return;

        if (state == KOOPA_STATE_SHELL_RUNNING)
        {
            bool runningTowardsMario = (vx > 0 && mario->x > this->x) || (vx < 0 && mario->x < this->x);
            if (runningTowardsMario)
            {
                mario->Hurt();
            }
        }
        else if (state == KOOPA_STATE_SHELL)
        {
            this->nx = (mario->x < this->x) ? 1 : -1;
            SetState(KOOPA_STATE_SHELL_RUNNING);
        }
        else if (state == KOOPA_STATE_WALKING)
        {
            mario->Hurt();
        }
    }
    else if (state == KOOPA_STATE_SHELL_RUNNING)
    {
        if (auto goomba = dynamic_cast<CGoomba *>(other))
        {
            goomba->SetState(GOOMBA_STATE_DIE);
            if (scene)
                scene->AddScore(100);
        }
        else if (auto koopa = dynamic_cast<CKoopa *>(other))
        {
            koopa->SetIsDead(true);
            CAudioManager::GetInstance()->Play("kick");
            if (scene)
                scene->AddScore(200);
        }
    }
}

void CKoopa::OnCollisionY(CGameObject *other, float ny)
{
    if (auto mario = dynamic_cast<CMario *>(other))
    {
        float ml, mb, mr, mt;
        mario->GetBoundingBox(ml, mb, mr, mt);
        float kl, kb, kr, kt;
        this->GetBoundingBox(kl, kb, kr, kt);

        if (mario->vy <= 0 && mb > kb)
        { // Mario dẫm lên đầu từ phía trên
            if (state == KOOPA_STATE_WALKING || state == KOOPA_STATE_SHELL_RUNNING)
            {
                SetState(KOOPA_STATE_SHELL);
                if (scene)
                    scene->AddScore(100);
                mario->StompBounce();
            }
            else if (state == KOOPA_STATE_SHELL)
            {
                this->nx = (mario->x < this->x) ? 1 : -1;
                SetState(KOOPA_STATE_SHELL_RUNNING);
                mario->StompBounce();
            }
        }
        else
        {
            // Hit from below or sides when falling
            if (state == KOOPA_STATE_WALKING)
            {
                mario->Hurt();
            }
            else if (state == KOOPA_STATE_SHELL_RUNNING)
            {
                bool runningTowardsMario = (vx > 0 && mario->x > this->x) || (vx < 0 && mario->x < this->x);
                if (runningTowardsMario)
                {
                    mario->Hurt();
                }
            }
        }
    }
}

bool CKoopa::IsBlocking(CGameObject *other)
{
    if (dynamic_cast<CMario *>(other))
    {
        return false;
    }
    if (state == KOOPA_STATE_SHELL_RUNNING && dynamic_cast<CGoomba *>(other))
    {
        return false; // don't block physics when plowing
    }
    return true;
}
