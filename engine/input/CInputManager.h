#pragma once
#include "KeyboardManager.h"
#include "InputState.h"

class CInputManager {
private:
    static CInputManager* instance;
    InputState state;
    CInputManager() {}

public:
    static CInputManager* GetInstance();
    void Update();
    const InputState& GetState() const { return state; }
};