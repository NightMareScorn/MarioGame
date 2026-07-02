#include "CLuckyBlock.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../../engine/audio/CAudioManager.h"
#include "../../scenes/play/CPlayScene.h"
#include "../../scenes/CSceneManager.h"
#include "../items/CCoin.h"
#include "../items/CMushroom.h"
#include "../items/CStar.h"
#include "../items/CFireFlower.h"

static const float LUCKY_BOUNCE_HEIGHT = 5.0f;
static const float LUCKY_BOUNCE_SPEED = 0.1f;

CLuckyBlock::CLuckyBlock(float x, float y, bool isEmpty)
    : CBlock(x, y), state(isEmpty ? ELuckyBlockState::EMPTY : ELuckyBlockState::IDLE),
      hiddenItem(nullptr), bounceStartY(y)
{
}

void CLuckyBlock::_SpawnHiddenItem()
{
    CPlayScene* playScene = (CPlayScene*)CSceneManager::GetInstance()->GetCurrentScene();
    CMario* mario = playScene ? playScene->GetPlayer() : nullptr;

    CGameObject* item = hiddenItem;
    bool isNewItem = false;

    if (item != nullptr)
    {
        // Nếu là Nấm mà Mario đã lớn -> Tạo Hoa mới
        if (dynamic_cast<CMushroom*>(item) && mario && mario->GetPower() != EMarioPower::SMALL)
        {
            item = new CFireFlower(x, y + 16.0f);
            isNewItem = true;
        }
    }
    else
    {
        // Mặc định ra Nấm nếu Mario nhỏ, ra Hoa nếu Mario lớn
        if (mario && mario->GetPower() == EMarioPower::SMALL)
            item = new CMushroom(x, y + 16.0f);
        else
            item = new CFireFlower(x, y + 16.0f);
        isNewItem = true;
    }

    if (item)
    {
        if (auto coin = dynamic_cast<CCoin*>(item))
        {
            CAudioManager::GetInstance()->Play("coin");
            coin->SetState(COIN_STATE_POPPING);
        }
        else
        {
            item->y = y + 16.0f;
            if (isNewItem && playScene) {
                playScene->AddItem(item);
            }
            if (auto mushroom = dynamic_cast<CMushroom*>(item))
            {
                mushroom->SetState(MUSHROOM_STATE_MOVING);
            }
            else if (auto star = dynamic_cast<CStar*>(item))
            {
                star->SetState(STAR_STATE_MOVING);
            }
            else if (auto flower = dynamic_cast<CFireFlower*>(item))
            {
                flower->SetState(FLOWER_STATE_VISIBLE);
            }
        }
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
        CAnimations::GetInstance()->Render("ANI_LUCKY_BOX_OW_IDLE", x, y);
    }
}

void CLuckyBlock::GetBoundingBox(float& left, float& bottom, float& right, float& top)
{
    const float BLOCK_W = 16.0f;
    const float BLOCK_H = 16.0f;
    left = x;
    bottom = y;
    right = x + BLOCK_W;
    top = y + BLOCK_H;
}

void CLuckyBlock::OnHitFromBelow(CGameObject* hitter)
{
    if (state != ELuckyBlockState::IDLE)
        return;

    CAudioManager::GetInstance()->Play("bump");
    bounceStartY = y;
    state = ELuckyBlockState::BOUNCING_UP;
}
