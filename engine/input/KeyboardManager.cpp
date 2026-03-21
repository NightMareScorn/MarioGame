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

bool KeyboardManager::IsValidKey(int key) const {
    return (key >= 0 && key < 256);
}

void KeyboardManager::SetKeyState(int key, bool down) {
    if (IsValidKey(key)) currentKeys[key] = down;
}

bool KeyboardManager::IsKeyDown(int key) {
    return IsValidKey(key) && currentKeys[key];
}

bool KeyboardManager::IsKeyPressed(int key) {
    return IsValidKey(key) && currentKeys[key] && !previousKeys[key];
}

bool KeyboardManager::IsKeyReleased(int key) {
    return IsValidKey(key) && !currentKeys[key] && previousKeys[key];
}