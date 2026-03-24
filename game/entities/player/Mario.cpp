#include "Mario.h"
#include "MarioConfig.h"
#include "../../../engine/input/InputManager.h"
#include "../../../engine/utils/debug.h"

void Mario::Update(float dt) {
    const auto& input = InputManager::GetInstance()->GetState();
    HandleInput(input,dt);
    ApplyPhysics(dt);

    x += vx * dt;
    y += vy * dt;

    if (y < MarioConfig::GROUND_Y) {
        y = MarioConfig::GROUND_Y;
        vy = 0;
    }

    if (input.left || input.right || input.jump) {
        DebugOut(L"[INPUT] Active - vx: %.2f, vy: %.2f\n", vx, vy);
    }
    DebugOut(L"[PHYSICS] Mario Pos: x=%.2f, y=%.2f\n", x, y);
}

void Mario::HandleInput(const InputState& input, float dt) {
    if (input.right) vx += MarioConfig::ACCEL_X * dt;
    else if (input.left) vx -= MarioConfig::ACCEL_X * dt;
    else vx = 0;

    if (input.jump && y <= MarioConfig::GROUND_Y) {
        vy = MarioConfig::JUMP_FORCE;
    }
}

void Mario::ApplyPhysics(float dt) {
    vy += MarioConfig::GRAVITY * dt;
    if (vx > MarioConfig::MAX_SPEED_X) vx = MarioConfig::MAX_SPEED_X;
    if (vx < -MarioConfig::MAX_SPEED_X) vx = -MarioConfig::MAX_SPEED_X;
}