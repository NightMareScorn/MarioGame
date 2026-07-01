#pragma once
#include "../../../engine/input/CInputManager.h"
#include "../../../engine/input/InputState.h"
#include "../CGameObject.h"
#include "MarioConfig.h"

class CMario : public CGameObject {
public:
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float& l, float& t, float& r, float& b) override;
    void UpdateState();
    void SetOnGround(bool v) { onGround = v; }
    bool IsOnGround() const { return onGround; }
    void SetMapWidth(float w) { mapWidth = w; }
    bool IsBig() const { return form == EMarioForm::BIG; }
    void SetForm(EMarioForm f) { form = f; }
    bool IsDead() const { return isDead; }
    EMarioState GetState() const { return state; }
    void StartDeath();

private:
    EMarioState state = EMarioState::IDLE;
    EMarioForm form = EMarioForm::SMALL;
    int nx = 1; // 1 for right, -1 for left
    bool onGround = false;
    bool isJumping = false;
    float jumpHoldTime = 0.0f;
    float jumpStartY = 0.0f;
    float mapWidth = 0.0f;
    float dieTimer = 0.0f;
    bool isDead = false;

    void HandleInput(const InputState& input, float dt);
    void ApplyPhysics(float dt);
};