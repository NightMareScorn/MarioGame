#include "CBrick.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../../engine/audio/CAudioManager.h"
#include "../../../engine/utils/debug.h"
#include "../../registry/CResourceRegistry.h"
#include "../player/CMario.h"
#include "../../scenes/play/CPlayScene.h"
#include "../../scenes/CSceneManager.h"
#include "CBrickDebris.h"

static const float BOUNCE_HEIGHT = 5.0f;
static const float BOUNCE_SPEED = 0.1f;

CBrick::CBrick(float x, float y, const std::string &aniName, bool isInvisible)
    : CBlock(x, y), state(EBrickState::IDLE), aniName(aniName), bounceStartY(y), isInvisible(isInvisible)
{
}

void CBrick::Render()
{
    if (state == EBrickState::BROKEN || isInvisible)
        return;

    CAnimations::GetInstance()->Render(aniName, x, y);
}

void CBrick::GetBoundingBox(float &left, float &bottom, float &right, float &top)
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

void CBrick::OnHitFromBelow(CGameObject *hitter)
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
        CMario *mario = dynamic_cast<CMario *>(hitter);
        if (mario && mario->IsBig())
        {
            state = EBrickState::BROKEN;
            CAudioManager::GetInstance()->Play("break");

            CPlayScene *playScene = (CPlayScene *)CSceneManager::GetInstance()->GetCurrentScene();
            if (playScene)
            {
                playScene->AddItem(new CBrickDebris(x + 4.0f, y + 8.0f, -0.04f, 0.12f)); // Trên trái
                playScene->AddItem(new CBrickDebris(x + 12.0f, y + 8.0f, 0.04f, 0.12f)); // Trên phải
                playScene->AddItem(new CBrickDebris(x + 4.0f, y + 4.0f, -0.03f, 0.07f)); // Dưới trái
                playScene->AddItem(new CBrickDebris(x + 12.0f, y + 4.0f, 0.03f, 0.07f)); // Dưới phải
            }
        }
        else
        {
            state = EBrickState::BOUNCING_UP;
            bounceStartY = y;
            CAudioManager::GetInstance()->Play("bump");
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