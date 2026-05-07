#pragma once

class CGameObject {
public:
    float x, y;    // Tọa độ (World Space)
    float vx, vy;  // Vận tốc
    float ax, ay;  // Gia tốc
    int nx;        // Hướng quay mặt (1: phải, -1: trái)

    CGameObject() : x(100), y(100), vx(0), vy(0), ax(0), ay(0), nx(1) {}
    virtual void OnHitFromBelow(CGameObject* hitter) {}
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void GetBoundingBox(float &l, float &t, float &r, float &b) = 0;
};