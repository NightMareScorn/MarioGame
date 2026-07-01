#pragma once
#include "../CGameObject.h"

#define MUSHROOM_BBOX_WIDTH 16.0f
#define MUSHROOM_BBOX_HEIGHT 16.0f

#define MUSHROOM_STATE_HIDDEN 100
#define MUSHROOM_STATE_MOVING 200

#define MUSHROOM_SPEED 0.05f
#define MUSHROOM_GRAVITY 0.002f

class CMushroom : public CGameObject {
    int state;
public:
    CMushroom(float x, float y);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float& left, float& bottom, float& right, float& top) override;
    void SetState(int s);

    bool IsBlocking(CGameObject* other) override { return (dynamic_cast<CMushroom*>(other) == nullptr); }
    void OnCollisionX(CGameObject* other, float nx) override;
    void OnCollisionY(CGameObject* other, float ny) override;
};
