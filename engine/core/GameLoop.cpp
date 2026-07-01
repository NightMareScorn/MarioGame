#include "GameLoop.h"
#include "Game.h"
#include "CTimer.h"
#include "../input/KeyboardManager.h"
#include "../input/CInputManager.h"
#include "../rendering/Textures.h"
#include "../rendering/Camera.h"
#include "../utils/debug.h"
#include <stdio.h>
#include "../../game/scenes/play/CPlayScene.h"

#define BACKGROUND_COLOR D3DXCOLOR(92.0f/255, 148.0f/255, 252.0f/255, 1.0f) // NES SMB1 sky blue (#5C94FC)

CPlayScene* scene = nullptr;
static std::string selectedLevel = "content/levels/level_1_1.csv";

void HandleDebugInput() {
#ifdef _DEBUG      
    auto kb = KeyboardManager::GetInstance();

    if (kb->IsKeyPressed('A')) DebugOut(L"[A] - JUST PRESSED\n");
    if (kb->IsKeyReleased('A')) DebugOut(L"[A] - RELEASED\n");

    if (kb->IsKeyPressed('D')) DebugOut(L"[D] - JUST PRESSED\n");
    if (kb->IsKeyReleased('D')) DebugOut(L"[D] - RELEASED\n");

    if (kb->IsKeyPressed(VK_SPACE)) DebugOut(L"[SPACE] - JUST PRESSED\n");
    if (kb->IsKeyReleased(VK_SPACE)) DebugOut(L"[SPACE] - RELEASED\n");
#endif
}

void Update(float dt)
{
    if (scene == nullptr) {
        scene = new CPlayScene(selectedLevel);
        scene->Load();
    }

    CInputManager::GetInstance()->Update();
    HandleDebugInput();

    scene->Update(dt);

    // Update keyboard state at the very end
    KeyboardManager::GetInstance()->Update();
}

void LoadAssets()
{
    // CPlayScene::Load() handles this via Registry
}

void RenderBackground(CGame* g, ID3D10RenderTargetView* pRenderTargetView, ID3D10Device* pD3DDevice)
{
    // Use GetClearColor() which reads the scene's configured background color
    D3DXCOLOR bgColor = BACKGROUND_COLOR; // Default sky blue
    if (scene != nullptr) {
        bgColor = scene->GetClearColor();
    }
    pD3DDevice->ClearRenderTargetView(pRenderTargetView, bgColor);
}

void RenderGame(CGame* g)
{
    if (scene != nullptr) {
        scene->Render();
    }
}

void Render()
{
    CGame* g = CGame::GetInstance();
    LoadAssets();

    ID3D10Device* pD3DDevice = g->GetDirect3DDevice();
    IDXGISwapChain* pSwapChain = g->GetSwapChain();
    ID3D10RenderTargetView* pRenderTargetView = g->GetRenderTargetView();
    ID3DX10Sprite* spriteHandler = g->GetSpriteHandler();

    RenderBackground(g, pRenderTargetView, pD3DDevice);

    spriteHandler->Begin(D3DX10_SPRITE_SORT_TEXTURE);

    FLOAT NewBlendFactor[4] = { 0,0,0,0 };
    pD3DDevice->OMSetBlendState(g->GetAlphaBlending(), NewBlendFactor, 0xffffffff);

    RenderGame(g);

    spriteHandler->End();

    // Render HUD
    if (scene != nullptr) {
        RECT rect;
        rect.left = 10;
        rect.top = 10;
        rect.right = g->GetBackBufferWidth();
        rect.bottom = g->GetBackBufferHeight();

        wchar_t buffer[256];
        std::string lvl = scene->GetLevelId();
        std::wstring wlvl(lvl.begin(), lvl.end());
        swprintf_s(buffer, L"MARIO\n%06d\n\nCOINS\n%02d\n\nWORLD\n%s\n\nTIME\n%03d", 
                    scene->GetScore(), scene->GetCoins(), wlvl.c_str(), scene->GetTime());
        g->DrawTextRaw(buffer, rect, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
    }

    pSwapChain->Present(0, 0);
}

int Run(std::string level)
{
    selectedLevel = level;
	MSG msg;
	int done = 0;

	CTimer time(60.0f);

	while (!done)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		time.Tick();

		while (time.ShouldUpdate())
		{
			Update(time.GetDeltaTime() * 1000.0f);
			time.OnUpdate();
			if (CGame::GetInstance()->IsExitLevelRequested()) done = 1;
		}

		Render();
		time.OnRender();

		time.PrintFPS();
	}

	return 1;
}