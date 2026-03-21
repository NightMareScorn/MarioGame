#pragma once
#include "KeyboardManager.h"
#include "InputState.h"

class InputManager {
private:
    static InputManager* instance;
    InputState state;
    InputManager() {}

public:
    static InputManager* GetInstance();
    void Update();
    const InputState& GetState() const { return state; }
};