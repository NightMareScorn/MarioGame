#include "CBrick.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../../engine/audio/CAudioManager.h"
#include "../../../engine/utils/debug.h"
#include "../../registry/CResourceRegistry.h"
#include "../player/CMario.h"

static const float BOUNCE_HEIGHT = 5.0f;
static const float BOUNCE_SPEED = 0.1f;

CBrick::CBrick(float x, float y, const std::string& aniName, bool isInvisible)
    : CBlock(x, y), state(EBrickState::IDLE), aniName(aniName), bounceStartY(y), isInvisible(isInvisible)
{
}

void CBrick::Render()
{
    if (state == EBrickState::BROKEN || isInvisible)
        return;

    CAnimations::GetInstance()->Render(aniName, x, y);
}

void CBrick::GetBoundingBox(float& left, float& bottom, float& right, float& top)
{
    if (state == EBrickState::BROKEN)
    {
        left = right = top = bottom = 0;
        return;
    }

    const float BLOCK_W = 16.0f;
    const float BLOCK_H = 16.0f;

    left = x;
    bottom = y;
    right = x + BLOCK_W;
    top = y + BLOCK_H;
}

void CBrick::OnHitFromBelow(CGameObject* hitter)
{
    if (state == EBrickState::BROKEN)
        return;

    if (isInvisible)
    {
        isInvisible = false;
        return;
    }

    if (state == EBrickState::IDLE)
    {
        CAudioManager::GetInstance()->Play("bump");
        CMario* mario = dynamic_cast<CMario*>(hitter);
        if (mario && mario->IsBig())
        {
            state = EBrickState::BROKEN;
        }
        else
        {
            state = EBrickState::BOUNCING_UP;
            bounceStartY = y;
        }
    }
}

// TODO: State BREAK của BRICK (chỉ thực hiện được sau khi có Mario lớn)
void CBrick::Update(float dt)
{
    if (state == EBrickState::BOUNCING_UP)
    {
        if (y < bounceStartY + BOUNCE_HEIGHT)
        {
            y += BOUNCE_SPEED * dt;
        }
        else
        {
            y = bounceStartY + BOUNCE_HEIGHT;
            state = EBrickState::BOUNCING_DOWN;
        }
    }
    else if (state == EBrickState::BOUNCING_DOWN)
    {
        if (y > bounceStartY)
        {
            y -= BOUNCE_SPEED * dt;
        }
        else
        {
            y = bounceStartY;
            state = EBrickState::IDLE;
        }
    }
}