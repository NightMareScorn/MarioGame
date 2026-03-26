#include "CInputManager.h"

CInputManager* CInputManager::instance = nullptr; 

CInputManager* CInputManager::GetInstance() {
    if (!instance) instance = new CInputManager();
    return instance;
}

void CInputManager::Update() {
    auto kb = KeyboardManager::GetInstance();
    state.left = kb->IsKeyDown('A');
    state.right = kb->IsKeyDown('D');
    state.jump = kb->IsKeyPressed(VK_SPACE);
}