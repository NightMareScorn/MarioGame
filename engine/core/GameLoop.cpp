#include "GameLoop.h"
#include "Game.h"
#include "../input/KeyboardManager.h"
#include "../input/InputManager.h"
#include "../utils/debug.h"
#include <stdio.h>
#include "../../game/entities/player/Mario.h"

#define BACKGROUND_COLOR D3DXCOLOR(200.0f/255, 200.0f/255, 255.0f/255, 1.0f)

Mario* mario = nullptr;

void Update(float dt)
{
    if (mario == nullptr) {
        mario = new Mario();
        mario->x = 100.0f;
        mario->y = 100.0f;
    }

    InputManager::GetInstance()->Update();

    if (mario != nullptr) {
        mario->Update((float)dt);
    }

    KeyboardManager::GetInstance()->Update(); 
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

int Run()
{
	MSG msg;
	int done = 0;

	LARGE_INTEGER timerFreq;
	QueryPerformanceFrequency(&timerFreq);

	LARGE_INTEGER timeNow;
	QueryPerformanceCounter(&timeNow);

	float accumulator = 0.0f;
	const float dt = 0.0166667f; // 60Hz

	int updateCount = 0;
	int renderCount = 0;
	float timeElapsed = 0.0f;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		LARGE_INTEGER timeNext;
		QueryPerformanceCounter(&timeNext);

		float frameTime = (float)(timeNext.QuadPart - timeNow.QuadPart) / timerFreq.QuadPart;
		timeNow = timeNext;

		// Clamp frameTime to max 0.25s
		if (frameTime > 0.25f)
			frameTime = 0.25f;

		accumulator += frameTime;

		while (accumulator >= dt)
		{
			Update(dt);
			updateCount++;
			accumulator -= dt;
		}

		Render();
		renderCount++;

		timeElapsed += frameTime;
		if (timeElapsed >= 1.0f)
		{
			// In FPS vào Terminal VS Code
			printf("MarioGame | Update: %d | Render: %d\n", updateCount, renderCount);
			updateCount = 0;
			renderCount = 0;
			timeElapsed -= 1.0f;
		}
	}

	return 1;
}