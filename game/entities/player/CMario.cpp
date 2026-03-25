#include "CMario.h"
#include "MarioConfig.h"
#include "../../../engine/input/CInputManager.h"
#include "../../../engine/utils/debug.h"
#include "../../../engine/core/Game.h"
#include "../../../engine/rendering/Textures.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/Graphics/Animations.h"

void CMario::Update(float dt) {
    const auto& input = CInputManager::GetInstance()->GetState();
    HandleInput(input,dt);
    ApplyPhysics(dt);

    x += vx * dt;
    y += vy * dt;

    if (y < MarioConfig::GROUND_Y) {
        y = MarioConfig::GROUND_Y;
        vy = 0;
    }

    UpdateState(input);
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

    if (input.jump && y <= MarioConfig::GROUND_Y) {
        vy = MarioConfig::JUMP_FORCE;
    }
}

void CMario::UpdateState(const InputState& input) {
    if (y > MarioConfig::GROUND_Y) {
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

#include "../../registry/CResourceRegistry.h"

void CMario::Render() {
    std::string aniName = "ANI_MARIO_IDLE";
    switch (state) {
        case EMarioState::WALK: aniName = "ANI_MARIO_WALK"; break;
        case EMarioState::JUMP: aniName = "ANI_MARIO_JUMP"; break;
        case EMarioState::SKID: aniName = "ANI_MARIO_SKID"; break;
        default: aniName = "ANI_MARIO_IDLE"; break;
    }

    CResourceRegistry::GetInstance()->RenderAnimation(aniName, x, y, nx);
}