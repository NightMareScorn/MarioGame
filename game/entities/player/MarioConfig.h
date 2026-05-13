#pragma once
namespace MarioConfig {
    const float GRAVITY = -0.0007f;
    const float ACCEL_X = 0.0006f; // x1.5 approx
    const float FRICTION = 0.0006f;
    const float MAX_SPEED_X = 0.11f; 
    const float JUMP_FORCE = 0.28f; // Initial push
    const float JUMP_HOLD_FORCE = 0.0006f; // Continuous push
    const float MAX_JUMP_HEIGHT = 70.0f; // pixels

    const int ANI_IDLE_RIGHT = 100;
    const int ANI_IDLE_LEFT = 101;
    const int ANI_WALK_RIGHT = 200;
    const int ANI_WALK_LEFT = 201;
    const int ANI_JUMP_RIGHT = 300;
    const int ANI_JUMP_LEFT = 301;
}

enum class EMarioState { 
    IDLE, 
    WALK, 
    JUMP, 
    FALL, 
    SKID, 
    DIE
};
