#include "InputManager.h"

InputManager* InputManager::instance = nullptr; 

InputManager* InputManager::GetInstance() {
    if (!instance) instance = new InputManager();
    return instance;
}

void InputManager::Update() {
    auto kb = KeyboardManager::GetInstance();
    state.left = kb->IsKeyDown('A');
    state.right = kb->IsKeyDown('D');
    state.jump = kb->IsKeyPressed(VK_SPACE);
}