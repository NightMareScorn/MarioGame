#pragma once

class CGameObject {
public:
    float x, y;    // Tọa độ (World Space)
    float vx, vy;  // Vận tốc
    float ax, ay;  // Gia tốc

    CGameObject() : x(100), y(100), vx(0), vy(0), ax(0), ay(0) {}
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
};