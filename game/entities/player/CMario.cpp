#include <cmath>
#include "CMario.h"
#include "MarioConfig.h"
#include "../../../engine/input/CInputManager.h"
#include "../../../engine/utils/debug.h"
#include "../../../engine/core/Game.h"
#include "../../../engine/rendering/Textures.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../scenes/play/CPlayScene.h"
#include "../../scenes/CSceneManager.h"
#include "../items/CFireball.h"

static const float MARIO_W = 12.0f;
static const float MARIO_H = 15.0f;

int CMario::lives = 3;
float CMario::checkpointX = 0;
float CMario::checkpointY = 0;
bool CMario::hasCheckpoint = false;

void CMario::Update(float dt) {
    if (state == EMarioState::DIE) {
        vy += MarioConfig::GRAVITY * dt;
        y += vy * dt;
        dieTimer -= dt;
        return;
    }

    if (untouchableTimer > 0) untouchableTimer -= dt;
    if (starTimer > 0) starTimer -= dt;

    const auto& input = CInputManager::GetInstance()->GetState();
    HandleInput(input, dt);
    ApplyPhysics(dt);
    onGround = false;
}

void CMario::HandleInput(const InputState& input, float dt) {
    if (input.right) {
        vx += MarioConfig::ACCEL_X * dt;
        if (vx >= 0) nx = 1;
    }
    else if (input.left) {
        vx -= MarioConfig::ACCEL_X * dt;
        if (vx <= 0) nx = -1;
    }
    else {
        if (vx > 0) {
            vx -= MarioConfig::FRICTION * dt;
            if (vx < 0) vx = 0;
        }
        else if (vx < 0) {
            vx += MarioConfig::FRICTION * dt;
            if (vx > 0) vx = 0;
        }
    }

    if (input.jump && onGround) {
        vy = MarioConfig::JUMP_FORCE;
    }

    if (input.attack && power == EMarioPower::FIRE) {
        TryShootFireball();
    }
}

void CMario::UpdateState() {
    if (state == EMarioState::DIE) return;

    EMarioState oldState = state;
    const auto& input = CInputManager::GetInstance()->GetState();

    if (!onGround) {
        if (vy > 0) state = EMarioState::JUMP;
        else state = EMarioState::FALL;
    }
    else {
        if (std::abs(vx) > 0.01f) {
            if ((vx > 0 && input.left) || (vx < 0 && input.right)) state = EMarioState::SKID;
            else state = EMarioState::WALK;
        }
        else {
            vx = 0;
            state = EMarioState::IDLE;
        }
    }
}

bool CMario::IsInvincible() const {
    return untouchableTimer > 0 || starTimer > 0;
}

void CMario::Die() {
    if (state == EMarioState::DIE) return;
    state = EMarioState::DIE;
    lives--; 
    vx = 0;
    vy = MarioConfig::JUMP_FORCE;
    dieTimer = 2000.0f; 
    DebugOut(L"[MARIO] Died. Remaining lives: %d\n", lives);
}

void CMario::Hurt() {
    if (IsInvincible() || state == EMarioState::DIE) return;

    if (power != EMarioPower::SMALL) {
        power = EMarioPower::SMALL;
        untouchableTimer = 2000.0f;
    } else {
        Die();
    }
}

void CMario::ApplyPhysics(float dt) {
    vy += MarioConfig::GRAVITY * dt;
    if (vx > MarioConfig::MAX_SPEED_X) vx = MarioConfig::MAX_SPEED_X;
    if (vx < -MarioConfig::MAX_SPEED_X) vx = -MarioConfig::MAX_SPEED_X;
}

void CMario::GetBoundingBox(float &left, float &bottom, float &right, float &top) {
    left = x;
    bottom = y;
    right = x + MARIO_W;
    top = (power == EMarioPower::SMALL) ? y + MARIO_H : y + 31.0f;
}

void CMario::GrowToBig() {
    if (power != EMarioPower::SMALL) return;
    this->y -= 16.0f; 
    this->power = EMarioPower::BIG;
}

void CMario::StompBounce() {
    this->vy = 0.22f;
}

void CMario::TryShootFireball() {
    // Giới hạn tốc độ bắn
    static DWORD lastShot = 0;
    if (GetTickCount() - lastShot < 300) return;
    lastShot = GetTickCount();

    CPlayScene* scene = dynamic_cast<CPlayScene*>(CSceneManager::GetInstance()->GetCurrentScene());
    if (scene) {
        CFireball* fb = new CFireball(x + (nx > 0 ? 10 : -5), y + 10.0f, nx);
        scene->AddItem(fb);
    }
}

void CMario::Render() {
    if (untouchableTimer > 0 || starTimer > 0) {
        if ((int(GetTickCount64()) / 50) % 2 == 0) return; 
    }
    std::string prefix = "ANI_MARIO_";
    if (power == EMarioPower::BIG) prefix = "ANI_BIG_MARIO_";
    else if (power == EMarioPower::FIRE) prefix = "ANI_FIRE_MARIO_";

    std::string aniName = prefix + "IDLE";
    if (state == EMarioState::DIE) {
        aniName = "ANI_MARIO_DIE";
    } else {
        if (!onGround) {
            aniName = prefix + "JUMP";
        } else {
            if (abs(vx) > 0.01f) aniName = prefix + "WALK";
            else aniName = prefix + "IDLE";
        }
    }

    CAnimations::GetInstance()->Render(aniName, x, y, nx);
}