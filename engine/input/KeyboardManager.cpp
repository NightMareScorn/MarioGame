#include "KeyboardManager.h"
#include <string.h>

KeyboardManager* KeyboardManager::instance = nullptr;

KeyboardManager::KeyboardManager() {
    memset(currentKeys, 0, sizeof(currentKeys));
    memset(previousKeys, 0, sizeof(previousKeys));
}

KeyboardManager* KeyboardManager::GetInstance() {
    if (instance == nullptr) instance = new KeyboardManager();
    return instance;
}

void KeyboardManager::Update() {
    memcpy(previousKeys, currentKeys, sizeof(currentKeys));
}

void KeyboardManager::SetKeyState(int key, bool down) {
    if (key >= 0 && key < 256) currentKeys[key] = down;
}

bool KeyboardManager::IsKeyDown(int key) {
    if (key < 0 || key >= 256) return false;
    return currentKeys[key];
}

bool KeyboardManager::IsKeyPressed(int key) {
    if (key < 0 || key >= 256) return false;
    return currentKeys[key] && !previousKeys[key];
}

bool KeyboardManager::IsKeyReleased(int key) {
    if (key < 0 || key >= 256) return false;
    return !currentKeys[key] && previousKeys[key];
}