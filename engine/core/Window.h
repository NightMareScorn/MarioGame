#pragma once
#include <windows.h>
#include "resource.h"

#define WINDOW_CLASS_NAME L"BaseEngineWindow"
#define MAIN_WINDOW_TITLE L"Mario"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight);

typedef void (*ResizeCallback)(int width, int height);
void SetResizeCallback(ResizeCallback callback);
