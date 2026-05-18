#include "GameLoop.h"
#include "Game.h"
#include "CTimer.h"
#include "../input/KeyboardManager.h"
#include "../input/CInputManager.h"
#include "../rendering/Textures.h"
#include "../rendering/Camera.h"
#include "../utils/debug.h"
#include <stdio.h>
#include "../../game/scenes/CSceneManager.h"
#include "../../game/scenes/play/CPlayScene.h"

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
    auto sceneManager = CSceneManager::GetInstance();
    if (sceneManager->GetCurrentScene() == nullptr) {
        sceneManager->SetScene(new CPlayScene());
    }

    CInputManager::GetInstance()->Update();
    HandleDebugInput();

    sceneManager->Update(dt);

    // Update keyboard state at the very end
    KeyboardManager::GetInstance()->Update();
}

void LoadAssets()
{
    // CPlayScene::Load() handles this via Registry
}

void RenderBackground(CGame* g, ID3D10RenderTargetView* pRenderTargetView, ID3D10Device* pD3DDevice)
{
    D3DXCOLOR clearColor = D3DXCOLOR(0, 0, 0, 1); // Default to black if no scene
    if (CSceneManager::GetInstance()->GetCurrentScene() != nullptr) {
        clearColor = ((CPlayScene*)CSceneManager::GetInstance()->GetCurrentScene())->GetClearColor();
    }
    pD3DDevice->ClearRenderTargetView(pRenderTargetView, clearColor);
}

void RenderGame(CGame* g)
{
    CSceneManager::GetInstance()->Render();
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