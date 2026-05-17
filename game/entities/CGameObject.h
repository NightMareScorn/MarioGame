#pragma once

class CGameObject {
public:
    float x, y;    // Tọa độ (World Space)
    float vx, vy;  // Vận tốc
    float ax, ay;  // Gia tốc
    int nx;        // Hướng quay mặt (1: phải, -1: trái)

    CGameObject() : x(100), y(100), vx(0), vy(0), ax(0), ay(0), nx(1) {}
    virtual ~CGameObject() {}
    virtual bool IsBlocking(CGameObject* other) { return true; }
    virtual void OnHitFromBelow(CGameObject* hitter) {}
    virtual void OnCollisionX(CGameObject* other, float nx) {}
    virtual void OnCollisionY(CGameObject* other, float ny) {}
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void GetBoundingBox(float &left, float &bottom, float &right, float &top) = 0;
};