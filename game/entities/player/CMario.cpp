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
    
    // Handle input before resetting ground state, so we know if we were on the ground last frame!
    HandleInput(input,dt);

    // Update state based on current velocities and ground status BEFORE clearing it
    UpdateState(input);

    // Reset ground state before applying physics/collision for the current frame
    SetOnGround(false);

    ApplyPhysics(dt);
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
        // Simple friction
        if (vx > 0) {
            vx -= MarioConfig::ACCEL_X * dt;
            if (vx < 0) vx = 0;
        }
        else if (vx < 0) {
            vx += MarioConfig::ACCEL_X * dt;
            if (vx > 0) vx = 0;
        }
    }

    if (input.jump && IsOnGround()) {
        vy = MarioConfig::JUMP_FORCE;
        SetOnGround(false);
    }
}

void CMario::UpdateState(const InputState& input) {
    if (!IsOnGround()) {
        state = EMarioState::JUMP;
    }
    else if (vx != 0) {
        // Check for skidding: moving one way but pressing the other
        if ((vx > 0 && input.left) || (vx < 0 && input.right)) {
            state = EMarioState::SKID;
        }
        else {
            state = EMarioState::WALK;
        }
    }
    else {
        state = EMarioState::IDLE;
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
        case EMarioState::SKID: aniName = "ANI_MARIO_SKID"; break;
        default: aniName = "ANI_MARIO_IDLE"; break;
    }

    CAnimations::GetInstance()->Render(aniName, x, y, nx);
}