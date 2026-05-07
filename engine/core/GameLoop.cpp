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
        scene = new CPlayScene();
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
    pD3DDevice->ClearRenderTargetView(pRenderTargetView, BACKGROUND_COLOR);
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
    pSwapChain->Present(0, 0);
}

int Run()
{
	MSG msg;
	int done = 0;

	CTimer time(60.0f);

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
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
		}

		Render();
		time.OnRender();

		time.PrintFPS();
	}

	return 1;
}