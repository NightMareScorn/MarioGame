#pragma once
#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>

#define WINDOW_CLASS_NAME L"BaseEngineWindow"
#define MAIN_WINDOW_TITLE L"Mario"
#define WINDOW_ICON_PATH L"mario.ico"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight);
