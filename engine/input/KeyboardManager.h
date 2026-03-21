#pragma once
#include <windows.h>

class KeyboardManager {
private:
    static KeyboardManager* instance;
    bool currentKeys[256];
    bool previousKeys[256];

    KeyboardManager();
    bool IsValidKey(int key) const;

public:
    static KeyboardManager* GetInstance();
    void Update();
    void SetKeyState(int key, bool down);

    bool IsKeyDown(int key);
    bool IsKeyPressed(int key);
    bool IsKeyReleased(int key);
};