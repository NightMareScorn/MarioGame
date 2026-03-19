#include "Mario.h"
#include <algorithm>
#include "../../../engine/Utils/debug.h"

void Mario::Update(float dt) {
    auto kb = KeyboardManager::GetInstance();

    // 1. Xử lý Di chuyển ngang (vX)
    if (kb->IsKeyDown(VK_RIGHT) || kb->IsKeyDown('D')) {
        vx += ACCEL_X * dt;
    }
    else if (kb->IsKeyDown(VK_LEFT) || kb->IsKeyDown('A')) {
        vx -= ACCEL_X * dt;
    }
    else {
        // Dừng lại ngay (Bạn có thể thêm ma sát ở đây sau)
        vx = 0;
    }

    // Giới hạn tốc độ chạy (Clamp)
    vx = std::clamp(vx, -MAX_SPEED_X, MAX_SPEED_X);

    // 2. Xử lý Trọng lực & Nhảy (vY)
    vy += GRAVITY * dt; // Luôn áp dụng trọng lực

    if (kb->IsKeyPressed(VK_SPACE) || kb->IsKeyPressed('W')) {
        // Chỉ cho nhảy nếu đang ở "mặt đất" (Tạm thời là y <= 100)
        if (y <= 100.0f) {
            vy = JUMP_FORCE;
        }
    }

    // Giới hạn tốc độ rơi (Không rơi quá nhanh gây xuyên nền)
    if (vy < MAX_FALL_SPEED) vy = MAX_FALL_SPEED;

    // 3. Cập nhật vị trí
    x += vx * dt;
    y += vy * dt;

    // 4. Giới hạn mặt đất tạm thời (Để Mario không rơi mất khỏi màn hình)
    if (y < 100.0f) {
        y = 100.0f;
        vy = 0;
    }

    DebugOut(L"[PHYSICS] Mario Pos: %.2f, %.2f | Vel: %.2f, %.2f\n", x, y, vx, vy);
}