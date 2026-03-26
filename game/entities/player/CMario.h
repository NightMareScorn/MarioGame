#pragma once
#include "../CGameObject.h"
#include "../../engine/input/CInputManager.h"
#include "../../engine/input/InputState.h"

enum class EMarioState {
    IDLE,
    WALK,
    JUMP,
    SKID
};

class CMario : public CGameObject {
public:
    void Update(float dt) override; 
    void Render() override;
private:
    EMarioState state = EMarioState::IDLE;
    int nx = 1; // 1 for right, -1 for left

    void HandleInput(const InputState& input, float dt);
    void ApplyPhysics(float dt);
    void UpdateState(const InputState& input);
};