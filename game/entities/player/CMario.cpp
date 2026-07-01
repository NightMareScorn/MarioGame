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

static const float MARIO_W = 12.0f; //14
static const float MARIO_H = 15.0f; //28

void CMario::Update(float dt) {
    if (state == EMarioState::DIE) {
        vy -= 0.0005f * dt; // Simple gravity for dying Mario
        y += vy * dt;
        dieTimer += dt;
        if (dieTimer > 3000) isDead = true; // 3 seconds before respawn
        return;
    }


    if (y < -32.0f && state != EMarioState::DIE) {
        StartDeath();
        return;
    }

    const auto& input = CInputManager::GetInstance()->GetState();
    HandleInput(input,dt);
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
        vy = MarioConfig::JUMP_FORCE; // 0.2f
        isJumping = true;
        jumpHoldTime = 0.0f;
        CAudioManager::GetInstance()->Play("jump");
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

    if (oldState != state) {
        DebugOut(L"[FSM] State Changed: %d\n", (int)state);
    }
}


void CMario::ApplyPhysics(float dt) {
    // 1. Gravity is ALWAYS applied
    vy += MarioConfig::GRAVITY * dt;
    
    // 2. Add extra upward force if still holding jump
    if (isJumping) {
        if (CInputManager::GetInstance()->GetState().jump) {
            jumpHoldTime += dt;
            if (jumpHoldTime < 200.0f) { // Max hold time ~ 200ms
                vy += MarioConfig::JUMP_HOLD_FORCE * dt;
            } else {
                isJumping = false; // Max height reached
            }
        } else {
            isJumping = false; // Released early, just stop adding hold force!
        }
    }

    if (vx > MarioConfig::MAX_SPEED_X) vx = MarioConfig::MAX_SPEED_X;
    if (vx < -MarioConfig::MAX_SPEED_X) vx = -MarioConfig::MAX_SPEED_X;

    // 1. Map boundaries
    if (x < 0) x = 0;
    if (mapWidth > 0 && x > mapWidth - MARIO_W) x = mapWidth - MARIO_W;

    // 2. Camera boundary (cannot go left of camera)
    float camX, camY;
    CCamera::GetInstance()->GetCamPos(camX, camY);
    if (x < camX) x = camX;
}

void CMario::GetBoundingBox(float &left, float &bottom, float &right, float &top) {
    left = x;
    bottom = y;
    right = x + MARIO_W;
    top = y + MARIO_H;
}


#include "../../registry/CResourceRegistry.h"

void CMario::StartDeath() {
    if (state == EMarioState::DIE) return;
    state = EMarioState::DIE;
    vy = 0.18f; // Stronger bounce up
    vx = 0;
    dieTimer = 0;
    CAudioManager::GetInstance()->Play("die");
}

void CMario::Render() {
    std::string aniName = "ANI_MARIO_IDLE";
    if (state == EMarioState::DIE) {
        if (vy > 0) aniName = "ANI_MARIO_JUMP";
        else aniName = "ANI_MARIO_DIE";
    }
    else {
        switch (state) {
            case EMarioState::WALK: aniName = "ANI_MARIO_WALK"; break;
            case EMarioState::JUMP: aniName = "ANI_MARIO_JUMP"; break;
            case EMarioState::FALL: aniName = "ANI_MARIO_JUMP"; break;
            case EMarioState::SKID: aniName = "ANI_MARIO_SKID"; break;
            default:                aniName = "ANI_MARIO_IDLE"; break;
        }
    }

    CAnimations::GetInstance()->Render(aniName, x, y, nx);
}