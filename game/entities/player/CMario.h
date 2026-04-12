#pragma once
#include "../CGameObject.h"
#include "../../engine/input/CInputManager.h"
#include "../../engine/input/InputState.h"
#include "MarioConfig.h"

class CMario : public CGameObject {
public:
    void Update(float dt) override; 
    void Render() override;
    void GetBoundingBox(float &l, float &t, float &r, float &b) override;
    void UpdateState();
    void SetOnGround(bool v) { onGround = v; }
    bool IsOnGround() const { return onGround; }
private:
    EMarioState state = EMarioState::IDLE;
    int nx = 1; // 1 for right, -1 for left
    bool onGround = false;

    void HandleInput(const InputState& input, float dt);
    void ApplyPhysics(float dt);
};