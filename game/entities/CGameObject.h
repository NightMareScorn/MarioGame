#pragma once
class CGameObject {
public:
    float x, y, vx, vy, ax, ay;
    int nx;
    bool isDead = false;

    CGameObject() : x(100), y(100), vx(0), vy(0), ax(0), ay(0), nx(1) {}
    virtual ~CGameObject() = default;

    virtual void SetState(int s) {} 
    virtual void SetPosition(float x, float y) { this->x = x; this->y = y; }
    virtual bool IsDead() const { return isDead; }
    virtual void SetIsDead(bool dead) { isDead = dead; }

    virtual void OnHitFromBelow(CGameObject* hitter) {}
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