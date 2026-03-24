#pragma once

class GameObject {
public:
    float x, y;    // Tọa độ (World Space)
    float vx, vy;  // Vận tốc
    float ax, ay;  // Gia tốc

    GameObject() : x(100), y(100), vx(0), vy(0), ax(0), ay(0) {}
    virtual void Update(float dt) = 0;
};