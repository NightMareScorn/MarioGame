#include "CMario.h"
#include "MarioConfig.h"
#include "../../../engine/input/CInputManager.h"
#include "../../../engine/utils/debug.h"
#include "../../../engine/core/Game.h"
#include "../../../engine/rendering/Textures.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/Graphics/Animations.h"

static const float MARIO_W = 14.0f;
static const float MARIO_H = 28.0f;

void CMario::Update(float dt) {
    const auto& input = CInputManager::GetInstance()->GetState();
    HandleInput(input,dt);
    ApplyPhysics(dt);
    onGround = false;
}

void CMario::HandleInput(const InputState& input, float dt) {
    if (input.right) {
        vx += MarioConfig::ACCEL_X * dt;
        nx = 1;
    }
    else if (input.left) {
        vx -= MarioConfig::ACCEL_X * dt;
        nx = -1;
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
}

void CMario::UpdateState() {
    EMarioState oldState = state;
    const auto& input = CInputManager::GetInstance()->GetState();

    if (!onGround) {
        if (vy > 0) state = EMarioState::JUMP;
        else state = EMarioState::FALL;
    }
    else {
        if (abs(vx) > 0.01f) {
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
    vy += MarioConfig::GRAVITY * dt;
    if (vx > MarioConfig::MAX_SPEED_X) vx = MarioConfig::MAX_SPEED_X;
    if (vx < -MarioConfig::MAX_SPEED_X) vx = -MarioConfig::MAX_SPEED_X;
}

void CMario::GetBoundingBox(float &l, float &t, float &r, float &b) {
    l = x;
    t = y;
    r = x + MARIO_W;
    b = y + MARIO_H;
}


#include "../../registry/CResourceRegistry.h"

void CMario::Render() {
    std::string aniName = "ANI_MARIO_IDLE";
    switch (state) {
        case EMarioState::WALK: aniName = "ANI_MARIO_WALK"; break;
        case EMarioState::JUMP: aniName = "ANI_MARIO_JUMP"; break;
        case EMarioState::FALL: aniName = "ANI_MARIO_JUMP"; break;
        case EMarioState::SKID: aniName = "ANI_MARIO_SKID"; break;
        case EMarioState::DIE:  aniName = "ANI_MARIO_DIE";  break;
        default:                aniName = "ANI_MARIO_IDLE"; break;
    }

    CAnimations::GetInstance()->Render(aniName, x, y, nx);
}