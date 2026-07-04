#include "Window.h"
#include "Game.h"
#include "../input/KeyboardManager.h"

static ResizeCallback _resizeCallback = nullptr;
void SetResizeCallback(ResizeCallback callback) { _resizeCallback = callback; }

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_KEYDOWN:
        KeyboardManager::GetInstance()->SetKeyState((int)wParam, true);
        break;
    case WM_KEYUP:
        KeyboardManager::GetInstance()->SetKeyState((int)wParam, false);
        break;
	case WM_SIZE:
		if (CGame::GetInstance()) {
			CGame::GetInstance()->ProcessResize(LOWORD(lParam), HIWORD(lParam));
		}
        if (_resizeCallback) {
            _resizeCallback(LOWORD(lParam), HIWORD(lParam));
        }
		break;
	case WM_SIZING:
	{
		LPRECT lpRect = (LPRECT)lParam;
		
		// Target client aspect
		float targetAspect = 256.0f / 240.0f;

		// Calculate window borders size
		RECT windowRect = { 0, 0, 256, 240 };
		AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
		int borderWidth = (windowRect.right - windowRect.left) - 256;
		int borderHeight = (windowRect.bottom - windowRect.top) - 240;

		int currentWidth = (lpRect->right - lpRect->left) - borderWidth;
		int currentHeight = (lpRect->bottom - lpRect->top) - borderHeight;

		if (wParam == WMSZ_LEFT || wParam == WMSZ_RIGHT || wParam == WMSZ_LEFT + WMSZ_TOP || wParam == WMSZ_RIGHT + WMSZ_TOP) {
			// Adjust height based on width
			lpRect->bottom = lpRect->top + (int)(currentWidth / targetAspect) + borderHeight;
		} else {
			// Adjust width based on height
			lpRect->right = lpRect->left + (int)(currentHeight * targetAspect) + borderWidth;
		}
	}
	break;
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 800; // Original Width
		lpMMI->ptMinTrackSize.y = 600; // Original Height
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
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

	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));

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
		DWORD ErrCode = GetLastError();
		wchar_t buf[256];
		wsprintf(buf, L"[ERROR] CreateWindow failed. GetLastError=%lu", ErrCode);
		OutputDebugString(buf);
		return nullptr;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}