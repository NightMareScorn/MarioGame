#include "CLuckyBlock.h"
#include "../../../engine/Graphics/Animations.h"
#include "../items/CCoin.h"
#include "../items/CMushroom.h"
#include "../items/CStar.h"
#include "../items/CFireFlower.h"

static const float LUCKY_BOUNCE_HEIGHT = 5.0f;
static const float LUCKY_BOUNCE_SPEED = 0.1f;

CLuckyBlock::CLuckyBlock(float x, float y)
    : CBlock(x, y), state(ELuckyBlockState::IDLE), hiddenItem(nullptr), bounceStartY(y)
{
}

// -----------------------------------------------------------------------
// Spawn / activate the item hidden inside the block.
// Called once when the bounce up animation finishes.
// -----------------------------------------------------------------------
void CLuckyBlock::_SpawnHiddenItem()
{
    if (hiddenItem == nullptr)
        return;

    if (auto coin = dynamic_cast<CCoin *>(hiddenItem))
    {
        coin->SetState(COIN_STATE_POPPING);
    }
    else if (auto mushroom = dynamic_cast<CMushroom *>(hiddenItem))
    {
        // Position the mushroom just above the block so it slides out
        mushroom->y = y + 16.0f;
        mushroom->SetState(MUSHROOM_STATE_MOVING);
    }
    else if (auto star = dynamic_cast<CStar *>(hiddenItem))
    {
        star->y = y + 16.0f;
        star->SetState(STAR_STATE_MOVING);
    }
    else if (auto flower = dynamic_cast<CFireFlower *>(hiddenItem))
    {
        flower->y = y + 16.0f;
        flower->SetState(FLOWER_STATE_VISIBLE);
    }
}

void CLuckyBlock::Update(float dt)
{
    if (state == ELuckyBlockState::BOUNCING_UP)
    {
        if (y < bounceStartY + LUCKY_BOUNCE_HEIGHT)
        {
            y += LUCKY_BOUNCE_SPEED * dt;
        }
        else
        {
            y = bounceStartY + LUCKY_BOUNCE_HEIGHT;
            state = ELuckyBlockState::BOUNCING_DOWN;

            // Spawn the hidden item now that the block has settled
            _SpawnHiddenItem();
        }
    }
    else if (state == ELuckyBlockState::BOUNCING_DOWN)
    {
        if (y > bounceStartY)
        {
            y -= LUCKY_BOUNCE_SPEED * dt;
        }
        else
        {
            y = bounceStartY;
            state = ELuckyBlockState::EMPTY;
        }
    }
}

void CLuckyBlock::Render()
{
    if (state == ELuckyBlockState::EMPTY)
    {
        CAnimations::GetInstance()->Render("ANI_LUCKY_BOX_OW_AFTER_HIT", x, y);
    }
    else
    {
        // IDLE, BOUNCING_UP, BOUNCING_DOWN — show the animated blinking sprite
        CAnimations::GetInstance()->Render("ANI_LUCKY_BOX_OW_IDLE", x, y);
    }
}

void CLuckyBlock::GetBoundingBox(float &left, float &bottom, float &right, float &top)
{
    const float BLOCK_W = 16.0f;
    const float BLOCK_H = 16.0f;
    left = x;
    bottom = y;
    right = x + BLOCK_W;
    top = y + BLOCK_H;
}

void CLuckyBlock::OnHitFromBelow(CGameObject *hitter)
{
    // Only react while in IDLE state; subsequent hits are ignored
    if (state != ELuckyBlockState::IDLE)
        return;

    bounceStartY = y;
    state = ELuckyBlockState::BOUNCING_UP;
}
