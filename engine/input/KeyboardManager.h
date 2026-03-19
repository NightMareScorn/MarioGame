#pragma once
#include <windows.h>

class KeyboardManager {
private:
    static KeyboardManager* instance;
    bool currentKeys[256];  // Trạng thái phím frame hiện tại
    bool previousKeys[256]; // Trạng thái phím frame trước đó

    KeyboardManager();

public:
    static KeyboardManager* GetInstance();

    // Copy current sang previous ở ĐẦU mỗi frame
    void Update(); 

    // Hàm nhận tín hiệu từ Windows Message Loop (WndProc)
    void SetKeyState(UINT key, bool down);

    // KIỂM TRA TRẠNG THÁI
    bool IsKeyDown(int key);     // Đang giữ phím (cho di chuyển)
    bool IsKeyPressed(int key);  // Vừa mới nhấn (cho nhảy)
    bool IsKeyReleased(int key); // Vừa mới thả (để test)
};