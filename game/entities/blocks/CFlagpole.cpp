#include "CFlagpole.h"
#include "../../../engine/Graphics/Animations.h"

CFlagpole::CFlagpole(float x, float y, std::string aniName) : CBlock(x, y) {
    this->aniName = aniName;
    this->state = FLAGPOLE_STATE_IDLE;
    // Lá cờ bắt đầu ở đỉnh cột (Cột cao khoảng 150px)
    this->flagY = y + 140.0f; 
    this->minFlagY = y + 16.0f;
}

void CFlagpole::Update(float dt) {
    if (state == FLAGPOLE_STATE_LOWERING) {
        if (flagY > minFlagY) {
            flagY -= 0.07f * dt; // Tốc độ cờ hạ xuống
        } else {
            flagY = minFlagY;
        }
    }
}

void CFlagpole::Render() {
    // 1. Thân cột (Vẽ 10 đoạn 16px đi lên từ vị trí chân x, y)
    for (int i = 0; i < 10; i++) {
        CAnimations::GetInstance()->Render(aniName, x, y + (i * 16.0f));
    }
    // 2. Đỉnh cột (Viên bi)
    CAnimations::GetInstance()->Render("ANI_FLAG_OW_TOP", x - 3.0f, y + 155.0f);
    // 3. Lá cờ (Di chuyển theo biến flagY)
    CAnimations::GetInstance()->Render("ANI_FLAG_OW", x - 12.0f, flagY);
}

void CFlagpole::GetBoundingBox(float &left, float &bottom, float &right, float &top) {
    left = x + 4.0f; 
    bottom = y;
    right = x + 12.0f;
    top = y + 160.0f; // Box cao để Mario nhảy chạm vào là dính cờ ngay
}