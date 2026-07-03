#include <cmath>
#include "CMario.h"
#include "MarioConfig.h"
#include "../../../engine/input/CInputManager.h"
#include "../../../engine/utils/debug.h"
#include "../../../engine/core/Game.h"
#include "../../../engine/rendering/Textures.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../../engine/audio/CAudioManager.h"
#include "../../scenes/play/CPlayScene.h"
#include "../../scenes/CSceneManager.h"
#include "../items/CFireball.h"

static const float MARIO_W = 12.0f;
static const float MARIO_H = 15.0f;

int CMario::lives = 3;
float CMario::checkpointX = 0;
float CMario::checkpointY = 0;
bool CMario::hasCheckpoint = false;
EMarioPower CMario::currentPower = EMarioPower::SMALL;

CMario::CMario() : CGameObject()
{
    this->power = currentPower;
}

void CMario::Update(float dt)
{
    if (isPoweringUp)
    {
        vx = 0.0f;
        vy = 0.0f;
        powerUpTimer += dt;
        if (powerUpTimer >= 600.0f)
        {
            isPoweringUp = false;
            this->y -= 16.0f;
            this->power = EMarioPower::BIG;
            this->SetInputLocked(false);
        }
        return;
    }

    if (state == EMarioState::DIE)
    {
        vy += MarioConfig::GRAVITY * dt;
        y += vy * dt;
        dieTimer -= dt;
        if (dieTimer <= 0)
        {
            isDead = true;
        }
        return;
    }

    if (y < -32.0f)
    {
        Die();
        return;
    }

    if (untouchableTimer > 0)
        untouchableTimer -= dt;
    if (starTimer > 0)
        starTimer -= dt;

    // --- XỬ LÝ LOGIC THẮNG MÀN ---
    if (state == EMarioState::GOAL_SLIDE)
    {
        vx = 0;
        vy = -0.05f; // Trượt xuống
        y += vy * dt;
        return;
    }
    else if (state == EMarioState::GOAL_WALK)
    {
        vx = 0.06f;
        nx = 1;
        x += vx * dt; // Di chuyển tự động
        vy += MarioConfig::GRAVITY * dt;
        y += vy * dt;
        if (y < 32.0f)
            y = 32.0f; // Bám sàn
        return;
    }
    else
    {
        if (!IsInputLocked())
        {
            HandleInput(CInputManager::GetInstance()->GetState(), dt);
        }
        ApplyPhysics(dt);
    }
    onGround = false;

    CMario::currentPower = this->power;
}

void CMario::HandleInput(const InputState &input, float dt)
{
    CPlayScene *scene = (CPlayScene *)CSceneManager::GetInstance()->GetCurrentScene();
    bool isUnderwater = (scene && scene->GetTheme() == "underwater");

    if (isUnderwater)
    {
        if (input.right)
        {
            vx += MarioConfig::ACCEL_X * 0.5f * dt;
            if (vx >= 0)
                nx = 1;
        }
        else if (input.left)
        {
            vx -= MarioConfig::ACCEL_X * 0.5f * dt;
            if (vx <= 0)
                nx = -1;
        }
        else
        {
            if (vx > 0)
            {
                vx -= MarioConfig::FRICTION * 1.5f * dt;
                if (vx < 0)
                    vx = 0;
            }
            else if (vx < 0)
            {
                vx += MarioConfig::FRICTION * 1.5f * dt;
                if (vx > 0)
                    vx = 0;
            }
        }

        if (input.jump && !wasJumpKeyPressed)
        {
            vy = 0.11f; // Swim flap upwards
            isJumping = false;
            CAudioManager::GetInstance()->Play("jump");
        }
        wasJumpKeyPressed = input.jump;
    }
    else
    {
        if (input.right)
        {
            vx += MarioConfig::ACCEL_X * dt;
            if (vx >= 0)
                nx = 1;
        }
        else if (input.left)
        {
            vx -= MarioConfig::ACCEL_X * dt;
            if (vx <= 0)
                nx = -1;
        }
        else
        {
            if (vx > 0)
            {
                vx -= MarioConfig::FRICTION * dt;
                if (vx < 0)
                    vx = 0;
            }
            else if (vx < 0)
            {
                vx += MarioConfig::FRICTION * dt;
                if (vx > 0)
                    vx = 0;
            }
        }

        if (input.jump && onGround)
        {
            vy = MarioConfig::JUMP_FORCE;
            isJumping = true;
            jumpHoldTime = 0.0f;
            CAudioManager::GetInstance()->Play("jump");
        }
        wasJumpKeyPressed = input.jump;
    }

    if (input.attack && (power == EMarioPower::SMALL_FIRE || power == EMarioPower::BIG_FIRE))
    {
        TryShootFireball();
    }
}

void CMario::UpdateState()
{
    if (state == EMarioState::DIE ||
        state == EMarioState::GOAL_SLIDE ||
        state == EMarioState::GOAL_WALK)
        return;

    EMarioState oldState = state;
    const auto &input = CInputManager::GetInstance()->GetState();

    if (!onGround)
    {
        if (vy > 0)
            state = EMarioState::JUMP;
        else
            state = EMarioState::FALL;
    }
    else
    {
        if (std::abs(vx) > 0.01f)
        {
            if ((vx > 0 && input.left) || (vx < 0 && input.right))
                state = EMarioState::SKID;
            else
                state = EMarioState::WALK;
        }
        else
        {
            vx = 0;
            state = EMarioState::IDLE;
        }
    }
}

void CMario::PowerUpFlower()
{
    if (this->power == EMarioPower::SMALL)
    {
        this->power = EMarioPower::SMALL_FIRE;
    }
    else if (this->power == EMarioPower::BIG)
    {
        this->power = EMarioPower::BIG_FIRE;
    }
    this->untouchableTimer = 500.0f;
}

void CMario::PowerUpMushroom()
{
    if (this->power == EMarioPower::SMALL)
    {
        this->isPoweringUp = true;
        this->powerUpTimer = 0.0f;
        this->SetInputLocked(true);
        this->vx = 0.0f;
        this->vy = 0.0f;
    }
    else if (this->power == EMarioPower::SMALL_FIRE)
    {
        this->y -= 16.0f;
        this->power = EMarioPower::BIG_FIRE;
    }
}

bool CMario::IsInvincible() const
{
    return untouchableTimer > 0 || starTimer > 0;
}

void CMario::Die()
{
    if (state == EMarioState::DIE)
        return;
    state = EMarioState::DIE;
    lives--;
    vx = 0;
    vy = MarioConfig::JUMP_FORCE;
    dieTimer = 2000.0f;
    CAudioManager::GetInstance()->Play("die");
}

void CMario::Hurt()
{

    if (IsInvincible() || state == EMarioState::DIE)
        return;
    if (power != EMarioPower::SMALL)
    {
        power = EMarioPower::SMALL;
        untouchableTimer = 2000.0f;
        CAudioManager::GetInstance()->Play("pipe-down"); // Chơi tạm âm thanh biến đổi
    }
    else
    {
        Die();
    }
}

void CMario::ApplyPhysics(float dt)
{
    CPlayScene *scene = (CPlayScene *)CSceneManager::GetInstance()->GetCurrentScene();
    bool isUnderwater = (scene && scene->GetTheme() == "underwater");

    if (isUnderwater)
    {
        vy += MarioConfig::GRAVITY * 0.15f * dt;
        if (vy < -0.04f)
            vy = -0.04f; // slow sink limit
    }
    else
    {
        vy += MarioConfig::GRAVITY * dt;

        if (isJumping)
        {
            if (CInputManager::GetInstance()->GetState().jump)
            {
                jumpHoldTime += dt;
                if (jumpHoldTime < 200.0f)
                {
                    vy += MarioConfig::JUMP_HOLD_FORCE * dt;
                }
                else
                {
                    isJumping = false;
                }
            }
            else
            {
                isJumping = false;
            }
        }
    }

    if (vx > MarioConfig::MAX_SPEED_X)
        vx = MarioConfig::MAX_SPEED_X;
    if (vx < -MarioConfig::MAX_SPEED_X)
        vx = -MarioConfig::MAX_SPEED_X;

    if (mapWidth > 0 && x > mapWidth - MARIO_W)
        x = mapWidth - MARIO_W;

    float camX, camY;
    CCamera::GetInstance()->GetCamPos(camX, camY);
    float renderOffsetX = CGame::GetInstance()->GetRenderOffsetX();
    float renderScale = CGame::GetInstance()->GetRenderScale();
    float xMin = camX - (renderScale > 0 ? renderOffsetX / renderScale : 0.0f);
    if (xMin < 0)
        xMin = 0;
    if (x < xMin)
        x = xMin;
}

void CMario::GetBoundingBox(float &left, float &bottom, float &right, float &top)
{
    left = x;
    bottom = y;
    right = x + MARIO_W;
    bool isTall = (power == EMarioPower::BIG || power == EMarioPower::BIG_FIRE);
    top = isTall ? y + 31.0f : y + 15.0f;
}

void CMario::GrowToBig()
{
    if (power != EMarioPower::SMALL)
        return;
    this->y -= 16.0f;
    this->power = EMarioPower::BIG;
}

void CMario::StompBounce() { this->vy = 0.22f; }

void CMario::TryShootFireball()
{
    static DWORD lastShot = 0;
    if (GetTickCount() - lastShot < 300)
        return;
    lastShot = GetTickCount();
    CPlayScene *playScene = dynamic_cast<CPlayScene *>(CSceneManager::GetInstance()->GetCurrentScene());
    if (playScene)
    {
        CFireball *fb = new CFireball(x + (nx > 0 ? 10 : -5), y + 10.0f, nx);
        playScene->AddItem(fb);
        CAudioManager::GetInstance()->Play("fireball"); // Nếu có
    }
}

void CMario::Render()
{
    if (untouchableTimer > 0 && starTimer <= 0)
    {
        if ((int(GetTickCount64()) / 50) % 2 == 0)
            return;
    }

    std::string aniName = "";

    if (isPoweringUp)
    {
        int cycle = (int)(powerUpTimer / 100.0f) % 2;
        std::string prefix = (cycle == 0) ? "ANI_BIG_MARIO_" : "ANI_MARIO_";
        aniName = prefix + "IDLE";
    }
    else if (starTimer > 0)
    {
        int colorFrame = (int(GetTickCount64() / 100) % 4) + 1;
        aniName = "ANI_STAR" + std::to_string(colorFrame) + "_OW";
    }
    else
    {
        CPlayScene *scene = (CPlayScene *)CSceneManager::GetInstance()->GetCurrentScene();
        bool isUnderwater = (scene && scene->GetTheme() == "underwater");

        if (isUnderwater && state != EMarioState::DIE)
        {
            aniName = IsBig() ? "ANI_MARIO_BIG_SWIM" : "ANI_MARIO_SWIM";
        }
        else
        {
            std::string prefix = "ANI_MARIO_";
            if (power == EMarioPower::BIG)
                prefix = "ANI_BIG_MARIO_";
            else if (power == EMarioPower::SMALL_FIRE)
                prefix = "ANI_SMALL_FIRE_MARIO_";
            else if (power == EMarioPower::BIG_FIRE)
                prefix = "ANI_BIG_FIRE_MARIO_";

            aniName = prefix + "IDLE";
            if (state == EMarioState::DIE)
                aniName = "ANI_MARIO_DIE";
            else if (!onGround)
                aniName = prefix + "JUMP";
            else if (state == EMarioState::SKID)
                aniName = prefix + "SKID";
            else if (abs(vx) > 0.01f)
                aniName = prefix + "WALK";
        }
    }

    CAnimations::GetInstance()->Render(aniName, x, y, nx);
}

void CMario::HitGoal()
{
    if (state == EMarioState::GOAL_SLIDE || state == EMarioState::GOAL_WALK)
        return;
    state = EMarioState::GOAL_SLIDE;
    this->SetInputLocked(true);
    vx = 0.0f;
    vy = -0.05f;
    nx = 1;
}

void CMario::SetState(int s)
{
    EMarioState newState = static_cast<EMarioState>(s);
    if (this->state == newState)
        return;
    this->state = newState;

    if (this->state == EMarioState::GOAL_WALK)
    {
        this->vx = 0.06f;
        this->nx = 1;
    }
}

void CMario::BecomeInvincible(float timeInSeconds)
{
    this->starTimer = timeInSeconds * 1000.0f;
    DebugOut(L"[POWERUP] Mario is now INVINCIBLE for 10s!\n");
}

bool CMario::IsBlocking(CGameObject *other)
{
    if (!other)
        return true;
    if (other->IsEnemy() || other->IsItem())
        return false;
    return true;
}