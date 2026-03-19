#pragma once
#include "../../../engine/core/GameObject.h"
#include "../../../engine/input/KeyboardManager.h"

class Mario : public GameObject {
private:
    const float GRAVITY = -0.0008f;      // Trọng lực kéo xuống (âm)
    const float ACCEL_X = 0.0006f;      // Gia tốc chạy
    const float MAX_SPEED_X = 0.25f;    // Tốc độ chạy tối đa
    const float MAX_FALL_SPEED = -0.5f; // Tốc độ rơi tối đa (âm)
    const float JUMP_FORCE = 0.45f;     // Lực nhảy (dương để bay lên)

public:
    void Update(float dt) override;
};