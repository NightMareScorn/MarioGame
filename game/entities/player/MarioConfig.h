#pragma once
namespace MarioConfig {
    const float GRAVITY = -0.0007f;
    const float ACCEL_X = 0.0008f;
    const float FRICTION = 0.0006f;
    const float MAX_SPEED_X = 0.15f;
    const float JUMP_FORCE = 0.35f;

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

enum class EMarioForm {
    SMALL,
    BIG
};
