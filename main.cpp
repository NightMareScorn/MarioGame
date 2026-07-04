#include "engine/core/Game.h"
#include "engine/core/GameLoop.h"
#include "engine/core/Window.h"
#include "engine/utils/debug.h"
#include <stdio.h>
#include <windows.h>
#include <wrl.h>
#include "WebView2.h"
#include "engine/utils/CScoreManager.h"
#include "engine/audio/CAudioManager.h"
#include <iomanip>

using namespace Microsoft::WRL;

// Pointer to WebViewController
static ComPtr<ICoreWebView2Controller> webviewController;
static ComPtr<ICoreWebView2> webviewWindow;
static bool isGameStarted = false;
static std::string selectedLevel = "";

void InitializeWebView(HWND hWnd) {
    // Set autoplay policy to bypass user gesture requirement
    SetEnvironmentVariableW(L"WEBVIEW2_ADDITIONAL_BROWSER_ARGUMENTS", L"--autoplay-policy=no-user-gesture-required");

    CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [hWnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                env->CreateCoreWebView2Controller(hWnd,
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [hWnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                            if (controller != nullptr) {
                                webviewController = controller;
                                webviewController->get_CoreWebView2(&webviewWindow);

                                // Play background music when WebView2 is initialized
                                CAudioManager::GetInstance()->PlayBGM("content/audio/overworld_theme.wav");
                            }

                            // Resize WebView to fit the bounds of the parent window
                            RECT bounds;
                            GetClientRect(hWnd, &bounds);
                            webviewController->put_Bounds(bounds);

                            // Schedule sub-level selection via message
                            webviewWindow->add_WebMessageReceived(
                                Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                                    [hWnd](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
                                        LPWSTR message;
                                        args->TryGetWebMessageAsString(&message);
                                        std::wstring wmsg(message);
                                        std::string msgStr;
                                        for (wchar_t c : wmsg) msgStr += (char)c;
                                        CoTaskMemFree(message);

                                        if (msgStr == "GET_SCORES") {
                                            // Build JSON manually since we don't have a library
                                            std::string json = "{";
                                            auto manager = CScoreManager::GetInstance();
                                            // Assuming level IDs are 1-1, 1-2, etc.
                                            const char* ids[] = { "1-1", "1-2", "1-3", "1-4", "2-1", "2-2", "2-3", "2-4" };
                                            for (int i = 0; i < 8; ++i) {
                                                auto best = manager->GetBest(ids[i]);
                                                json += "\"" + std::string(ids[i]) + "\":{\"score\":" + std::to_string(best.score) + ",\"time\":\"" + best.time + "\"}";
                                                if (i < 7) json += ",";
                                            }
                                            json += "}";
                                            
                                            std::wstring wjson;
                                            for (char c : json) wjson += (wchar_t)c;
                                            webview->PostWebMessageAsJson(wjson.c_str());
                                        } else {
                                            selectedLevel = msgStr;
                                            isGameStarted = true;
                                            webviewController->put_IsVisible(FALSE);

                                            // Stop menu background music when game starts
                                            CAudioManager::GetInstance()->StopBGM();
                                        }
                                        return S_OK;
                                    }).Get(), nullptr);

                            // Load local Menu using Virtual Host Mapping
                            wchar_t path[MAX_PATH];
                            GetModuleFileNameW(NULL, path, MAX_PATH);
                            std::wstring ws(path);
                            std::wstring dir = ws.substr(0, ws.find_last_of(L"\\/"));
                            std::wstring uiFolder = dir + L"\\ui";

                            // Map app.mario.local to our local ui folder
                            // This requires ICoreWebView2_3 interface
                            ComPtr<ICoreWebView2_3> webview3;
                            if (SUCCEEDED(webviewWindow->QueryInterface(IID_PPV_ARGS(&webview3)))) {
                                webview3->SetVirtualHostNameToFolderMapping(
                                    L"app.mario.local", uiFolder.c_str(),
                                    COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);
                            }

                            webviewWindow->Navigate(L"https://app.mario.local/index.html");
                            return S_OK;
                        }).Get());
                return S_OK;
            }).Get());
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  AllocConsole();
  FILE *fDummy;
  freopen_s(&fDummy, "CONOUT$", "w", stdout);
  freopen_s(&fDummy, "CONOUT$", "w", stderr);
  HWND hWnd = CreateGameWindow(hInstance, nCmdShow, 800, 600); // Larger window for menu
  SetDebugWindow(hWnd);

  LPGAME game = CGame::GetInstance();
  game->Init(hWnd, hInstance);

  CScoreManager::GetInstance()->Load();

  SetResizeCallback([](int w, int h) {
      if (webviewController) {
          RECT bounds = { 0, 0, w, h };
          webviewController->put_Bounds(bounds);
      }
  });

  std::string cmd = lpCmdLine;
  size_t pos = cmd.find("--level ");
  if (pos != std::string::npos) {
      selectedLevel = cmd.substr(pos + 8);
      isGameStarted = true;
  } else {
      InitializeWebView(hWnd);
  }

  // Application Loop
  while (true) {
      isGameStarted = false;
      if (webviewController) {
          webviewController->put_IsVisible(TRUE);
          // Play background music when returning to menu
          CAudioManager::GetInstance()->PlayBGM("content/audio/overworld_theme.wav");
      }
      CGame::GetInstance()->SetExitLevel(false);

      // Main Message Loop for Menu
      MSG msg;
      bool quit = false;
      while (GetMessage(&msg, NULL, 0, 0)) {
          TranslateMessage(&msg);
          DispatchMessage(&msg);

          if (isGameStarted) break;
          if (msg.message == WM_QUIT) { quit = true; break; }
      }

      if (quit) break;

      if (selectedLevel.empty()) selectedLevel = "content/levels/level_1_1.csv";
      if (selectedLevel.find("content/levels/") == std::string::npos) {
          selectedLevel = "content/levels/" + selectedLevel;
      }

      Run(selectedLevel);
  }

  return 0;
}