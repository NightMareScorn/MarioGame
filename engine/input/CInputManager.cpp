#include "CInputManager.h"

CInputManager* CInputManager::instance = nullptr; 

CInputManager* CInputManager::GetInstance() {
    if (!instance) instance = new CInputManager();
    return instance;
}

void CInputManager::Update() {
    auto kb = KeyboardManager::GetInstance();
    state.left = kb->IsKeyDown('A') || kb->IsKeyDown(VK_LEFT);
    state.right = kb->IsKeyDown('D') || kb->IsKeyDown(VK_RIGHT);
    state.jump = kb->IsKeyPressed(VK_SPACE) || kb->IsKeyPressed('K') || kb->IsKeyPressed('X') || kb->IsKeyPressed(VK_UP);
    state.attack = kb->IsKeyPressed('J') || kb->IsKeyPressed('Z');
}