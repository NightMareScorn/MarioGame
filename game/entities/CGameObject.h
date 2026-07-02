#pragma once

#include <string>
class CGameObject {
public:
    float x, y;    // Tọa độ (World Space)
    float vx, vy;  // Vận tốc
    float ax, ay;  // Gia tốc
    int nx;        // Hướng quay mặt (1: phải, -1: trái)
    std::string type;
    std::string spriteName;
    bool isDead = false;

    CGameObject() : x(100), y(100), vx(0), vy(0), ax(0), ay(0), nx(1) {}
    CGameObject(float x, float y) : x(x), y(y), vx(0), vy(0), ax(0), ay(0), nx(1) {}
    virtual ~CGameObject() {}
    virtual bool IsSolid() { return true; }
    virtual bool IsBlocking(CGameObject* other) { return true; }
    virtual void OnCollision(CGameObject* other) {}

    virtual void SetState(int s) {}
    virtual void SetPosition(float x, float y) { this->x = x; this->y = y; }
    virtual bool IsDead() const { return isDead; }
    virtual void SetIsDead(bool dead) { isDead = dead; }

    virtual void OnHitFromBelow(CGameObject* hitter) {}
    virtual void OnCollisionX(CGameObject* other, float nx) {}
    virtual void OnCollisionY(CGameObject* other, float ny) {}
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void GetBoundingBox(float &left, float &bottom, float &right, float &top) = 0;

    virtual bool IsEnemy() const { return false; }
    virtual bool IsItem() const { return false; }
    virtual bool ShouldRemove() const { return IsDead(); }
    virtual void OnStomped() {}
    virtual void OnFireballHit() {}
    virtual void OnCollected(class CMario* mario) {}
};