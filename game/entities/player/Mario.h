#pragma once
#include "../../../engine/core/GameObject.h"
#include "../../../engine/input/KeyboardManager.h"
#include "../../../engine/input/InputState.h"

class Mario : public GameObject {
public:
    void Update(float dt) override; 
private:
    void HandleInput(const InputState& input, float dt);
    void ApplyPhysics(float dt);
};