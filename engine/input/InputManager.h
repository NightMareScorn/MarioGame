#pragma once
#include "KeyboardManager.h"
#include "InputState.h"

class InputManager {
private:
    static InputManager* instance;
    InputState state;
    InputManager() {}

public:
    static InputManager* GetInstance() {
        if (!instance) instance = new InputManager();
        return instance;
    }

    void Update() {
        auto kb = KeyboardManager::GetInstance();
        state.left = kb->IsKeyDown('A');
        state.right = kb->IsKeyDown('D');
        state.jump = kb->IsKeyPressed(VK_SPACE);
    }

    const InputState& GetState() const { return state; }
};