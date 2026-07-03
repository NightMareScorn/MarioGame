#pragma once
#include "../CGameObject.h"
#include "../../registry/CResourceRegistry.h"
#include "../player/CMario.h"
#include "../../scenes/play/CPlayScene.h"
#include "../../scenes/CSceneManager.h"

#define BLOOPER_STATE_SINK 100
#define BLOOPER_STATE_SWIM 200

class CBlooper : public CGameObject {
    int state;
    float timer = 0.0f;
public:
    CBlooper(float x, float y);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
    void OnCollisionX(CGameObject *other, float nx) override;
    void OnCollisionY(CGameObject *other, float ny) override;

    bool IsSolid() override { return false; }
    bool IsBlocking(CGameObject *other) override { return false; }
    bool IsEnemy() const override { return true; }
};
