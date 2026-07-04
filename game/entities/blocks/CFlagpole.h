#pragma once
#include "CBlock.h"
#include <string>

#define FLAGPOLE_STATE_IDLE 100
#define FLAGPOLE_STATE_LOWERING 200

class CFlagpole : public CBlock {
    std::string aniName; // Lưu tên animation truyền từ MapLoader
    int state;
    float flagY;      // Tọa độ Y hiện tại của lá cờ
    float minFlagY;   // Tọa độ Y thấp nhất (chân cột)
public:
    // Sửa lại Constructor nhận 3 tham số để hết lỗi biên dịch
    CFlagpole(float x, float y, std::string aniName); 
    
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
    void SetState(int s) override { state = s; }
};