#include "GameLoop.h"
#include "Game.h"
#include "../input/KeyboardManager.h"
#include "../input/InputManager.h"
#include "../rendering/Textures.h"
#include "../rendering/Camera.h"
#include "../utils/debug.h"

#define BACKGROUND_COLOR D3DXCOLOR(200.0f/255, 200.0f/255, 255.0f/255, 1.0f)

void HandleDebugInput() {
#ifdef _DEBUG
    auto kb = KeyboardManager::GetInstance();

    // --- Debug phím A ---
    if (kb->IsKeyPressed('A'))    DebugOut(L"[A] - JUST PRESSED\n");
    if (kb->IsKeyDown('A'))      DebugOut(L"[A] - HOLDING\n");
    if (kb->IsKeyReleased('A'))   DebugOut(L"[A] - RELEASED\n");

    // --- Debug phím D ---
    if (kb->IsKeyPressed('D'))    DebugOut(L"[D] - JUST PRESSED\n");
    if (kb->IsKeyDown('D'))      DebugOut(L"[D] - HOLDING\n");
    if (kb->IsKeyReleased('D'))   DebugOut(L"[D] - RELEASED\n");

    // --- Debug phím Space ---
    if (kb->IsKeyPressed(VK_SPACE))  DebugOut(L"[SPACE] - JUST PRESSED\n");
    if (kb->IsKeyDown(VK_SPACE))    DebugOut(L"[SPACE] - HOLDING\n");
    if (kb->IsKeyReleased(VK_SPACE)) DebugOut(L"[SPACE] - RELEASED\n");
#endif
}

// --- Player state ---
float marioX = 300.0f;
float marioY = 100.0f;
const float MARIO_SPEED = 0.2f;

void UpdateInput()
{
    InputManager::GetInstance()->Update();
    KeyboardManager::GetInstance()->Update();
    HandleDebugInput();
}

void UpdatePlayer(DWORD dt)
{
    auto state = InputManager::GetInstance()->GetState();
    if (state.left) marioX -= MARIO_SPEED * dt;
    if (state.right) marioX += MARIO_SPEED * dt;
}

void UpdateCamera(DWORD dt)
{
    CCamera::GetInstance()->Update(marioX, marioY, dt);
}

void Update(DWORD dt)
{
    UpdateInput();
    UpdatePlayer(dt);
    UpdateCamera(dt);
}

void LoadAssets()
{
    static bool assetsLoaded = false;
    if (assetsLoaded) return;

    CTextures::GetInstance()->Add(1, L"content/textures/mario.png");
    CTextures::GetInstance()->Add(2, L"content/textures/misc.png");
    assetsLoaded = true;
}

void RenderBackground(CGame* g, ID3D10RenderTargetView* pRenderTargetView, ID3D10Device* pD3DDevice)
{
    pD3DDevice->ClearRenderTargetView(pRenderTargetView, BACKGROUND_COLOR);
}

void RenderPlayer(CGame* g)
{
    LPTEXTURE texMario = CTextures::GetInstance()->Get(1);
    float camX, camY;
    CCamera::GetInstance()->GetCamPos(camX, camY);

    // CSS: -123px 0; width: 6px; height: 8px;
    RECT rect;
    rect.left = 123;
    rect.top = 0;
    rect.right = 129;
    rect.bottom = 8;

    // Draw scale: 4x (24x32)
    g->Draw(marioX - camX, marioY - camY, texMario, &rect, 1.0f, 24, 32);
}

void RenderEnvironment(CGame* g)
{
    LPTEXTURE texMisc = CTextures::GetInstance()->Get(2);
    float camX, camY;
    CCamera::GetInstance()->GetCamPos(camX, camY);

    // Block CSS: -150 -58; Assume 16x16
    RECT rect;
    rect.left = 150;
    rect.top = 58;
    rect.right = 166;
    rect.bottom = 74;

    // Draw a row of blocks for camera testing
    for (int i = 0; i < 20; i++)
    {
        float blockX = i * 48.0f; // 16x3 = 48
        float blockY = 50.0f;
        g->Draw(blockX - camX, blockY - camY, texMisc, &rect, 1.0f, 48, 48);
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

    RenderEnvironment(g);
    RenderPlayer(g);

    spriteHandler->End();
    pSwapChain->Present(0, 0);
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
			Render();
		}
		else
			Sleep(tickPerFrame - dt);
	}

	return 1;
}