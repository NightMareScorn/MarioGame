#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>

#include "Engine/Utils/debug.h"
#include "Engine/Core/Game.h"
#include "engine/input/KeyboardManager.h"
#include "game/entities/player/Mario.h"

#define WINDOW_CLASS_NAME L"BaseEngineWindow"
#define MAIN_WINDOW_TITLE L"Mario Game Base Engine"
#define WINDOW_ICON_PATH L"mario.ico"

#define BACKGROUND_COLOR D3DXCOLOR(200.0f/255, 200.0f/255, 255.0f/255, 1.0f)

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

Mario* mario = nullptr;

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		// Task 11 -------------------------------------------------
	case WM_KEYDOWN:
		KeyboardManager::GetInstance()->SetKeyState((UINT)wParam, true);
		break;
	case WM_KEYUP:
		KeyboardManager::GetInstance()->SetKeyState((UINT)wParam, false);
		break;
		// Task 11 -------------------------------------------------
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Update(DWORD dt)
{
	// TODO: Handle updates for game objects / scene management
	// Task 11 -------------------------------------------------
	auto kb = KeyboardManager::GetInstance();

	// TEST NHẤN (Chỉ hiện 1 lần duy nhất khi vừa bấm xuống)
	if (kb->IsKeyPressed(VK_SPACE)) {
		DebugOut(L"[TEST] Space JUST PRESSED\n");
	}
	if (kb->IsKeyPressed(VK_LEFT)) {
		DebugOut(L"[TEST] Left JUST PRESSED\n");
	}
	if (kb->IsKeyPressed(VK_RIGHT)) {
		DebugOut(L"[TEST] Right JUST PRESSED\n");
	}

	// TEST GIỮ (Hiện liên tục khi đang đè phím)
	if (kb->IsKeyDown(VK_RIGHT)) {
		DebugOut(L"[TEST] Holding RIGHT\n");
	}

	if (kb->IsKeyDown(VK_LEFT)) {
		DebugOut(L"[TEST] Holding LEFT\n");
	}

	if (kb->IsKeyDown(VK_SPACE)) {
		DebugOut(L"[TEST] Holding SPACE\n");
	}
	// TEST THẢ (Chỉ hiện 1 lần khi vừa buông tay)
	if (kb->IsKeyReleased(VK_RIGHT)) {
		DebugOut(L"[TEST] RIGHT RELEASED\n");
	}

	if (kb->IsKeyReleased(VK_LEFT)) {
		DebugOut(L"[TEST] LEFT RELEASED\n");
	}

	if (kb->IsKeyReleased(VK_SPACE)) {
		DebugOut(L"[TEST] SPACE RELEASED\n");
	}
	//Task 11 -------------------------------------------------
	//Task 14 -------------------------------------------------
	mario->Update((float)dt);
	//Task 14 -------------------------------------------------
}

void Render()
{
	CGame* g = CGame::GetInstance();

	ID3D10Device* pD3DDevice = g->GetDirect3DDevice();
	IDXGISwapChain* pSwapChain = g->GetSwapChain();
	ID3D10RenderTargetView* pRenderTargetView = g->GetRenderTargetView();
	ID3DX10Sprite* spriteHandler = g->GetSpriteHandler();

	pD3DDevice->ClearRenderTargetView(pRenderTargetView, BACKGROUND_COLOR);

	spriteHandler->Begin(D3DX10_SPRITE_SORT_TEXTURE);

	FLOAT NewBlendFactor[4] = { 0,0,0,0 };
	pD3DDevice->OMSetBlendState(g->GetAlphaBlending(), NewBlendFactor, 0xffffffff);

	// TODO: Draw logic here! e.g., sprites, tiles...

	spriteHandler->End();
	pSwapChain->Present(0, 0);
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = (HICON)LoadImage(hInstance, WINDOW_ICON_PATH, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, 
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			ScreenWidth,
			ScreenHeight,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd) 
	{
		OutputDebugString(L"[ERROR] CreateWindow failed");
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int Run()
{
	MSG msg;
	int done = 0;
	ULONGLONG frameStart = GetTickCount64();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ULONGLONG now = GetTickCount64();
		DWORD dt = (DWORD)(now - frameStart);

		if (dt >= tickPerFrame)
		{
			frameStart = now;
			Update(dt);
			KeyboardManager::GetInstance()->Update(); // Task 11
			Render();
		}
		else
			Sleep(tickPerFrame - dt);	
	}

	return 1;
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
) {
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

	SetDebugWindow(hWnd);

	LPGAME game = CGame::GetInstance();
	game->Init(hWnd, hInstance);
	mario = new Mario();

	SetWindowPos(hWnd, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	Run();

	return 0;
}
