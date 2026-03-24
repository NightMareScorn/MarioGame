#include "engine/core/Game.h"
#include "engine/core/GameLoop.h"
#include "engine/core/Window.h"
#include "engine/utils/debug.h"
#include <stdio.h>
#include <windows.h>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  AllocConsole();
  FILE *fDummy;
  freopen_s(&fDummy, "CONOUT$", "w", stdout);
  freopen_s(&fDummy, "CONOUT$", "w", stderr);
  HWND hWnd = CreateGameWindow(hInstance, nCmdShow, 640, 480);
  SetDebugWindow(hWnd);

  LPGAME game = CGame::GetInstance();
  game->Init(hWnd, hInstance);

  SetWindowPos(hWnd, 0, 0, 0, 640, 480,
               SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
  Run();
  return 0;
}