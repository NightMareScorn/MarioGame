#pragma once
#include "../CGameObject.h"
#include "../../registry/CResourceRegistry.h"
#include "../player/CMario.h"
#include "../../scenes/play/CPlayScene.h"
#include "../../scenes/CSceneManager.h"

#define PIRANHA_STATE_RETRACTED 100
#define PIRANHA_STATE_EMERGING 200
#define PIRANHA_STATE_EMERGED 300
#define PIRANHA_STATE_RETRACTING 400

class CPiranhaPlant : public CGameObject {
    int state;
    float start_y;
    float timer;
public:
    CPiranhaPlant(float x, float y);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;

    bool IsSolid() override { return false; }
    bool IsBlocking(CGameObject *other) override { return false; }
    bool IsEnemy() const override { return true; }
};
