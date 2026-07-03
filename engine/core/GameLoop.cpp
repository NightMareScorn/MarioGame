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

CPlayScene *scene = nullptr;
static std::string selectedLevel = "content/levels/level_1_1.csv";

void HandleDebugInput()
{
#ifdef _DEBUG
    auto kb = KeyboardManager::GetInstance();

    if (kb->IsKeyPressed('A'))
        DebugOut(L"[A] - JUST PRESSED\n");
    if (kb->IsKeyReleased('A'))
        DebugOut(L"[A] - RELEASED\n");

    if (kb->IsKeyPressed('D'))
        DebugOut(L"[D] - JUST PRESSED\n");
    if (kb->IsKeyReleased('D'))
        DebugOut(L"[D] - RELEASED\n");

    if (kb->IsKeyPressed(VK_SPACE))
        DebugOut(L"[SPACE] - JUST PRESSED\n");
    if (kb->IsKeyReleased(VK_SPACE))
        DebugOut(L"[SPACE] - RELEASED\n");
#endif
}

void Update(float dt)
{
    auto sceneManager = CSceneManager::GetInstance();
    if (sceneManager->GetCurrentScene() == nullptr)
    {
        sceneManager->SetScene(new CPlayScene(selectedLevel));
    }
    scene = (CPlayScene *)sceneManager->GetCurrentScene();

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

void RenderBackground(CGame *g, ID3D10RenderTargetView *pRenderTargetView, ID3D10Device *pD3DDevice)
{
    D3DXCOLOR bgColor = D3DXCOLOR(92.0f / 255, 148.0f / 255, 252.0f / 255, 1.0f); // NES SMB1 sky blue (#5C94FC)
    if (CSceneManager::GetInstance()->GetCurrentScene() != nullptr)
    {
        bgColor = ((CPlayScene *)CSceneManager::GetInstance()->GetCurrentScene())->GetClearColor();
    }
    pD3DDevice->ClearRenderTargetView(pRenderTargetView, bgColor);
}

void RenderGame(CGame *g)
{
    CSceneManager::GetInstance()->Render();
}

void Render()
{
    CGame *g = CGame::GetInstance();
    LoadAssets();

    ID3D10Device *pD3DDevice = g->GetDirect3DDevice();
    IDXGISwapChain *pSwapChain = g->GetSwapChain();
    ID3D10RenderTargetView *pRenderTargetView = g->GetRenderTargetView();
    ID3DX10Sprite *spriteHandler = g->GetSpriteHandler();

    RenderBackground(g, pRenderTargetView, pD3DDevice);

    spriteHandler->Begin(D3DX10_SPRITE_SORT_TEXTURE);

    FLOAT NewBlendFactor[4] = {0, 0, 0, 0};
    pD3DDevice->OMSetBlendState(g->GetAlphaBlending(), NewBlendFactor, 0xffffffff);

    RenderGame(g);

    spriteHandler->End();

    // Render HUD
    if (scene != nullptr)
    {
        wchar_t textScore[64];
        wchar_t textCoins[64];
        wchar_t textWorld[64];
        wchar_t textTime[64];
        wchar_t textLives[64];

        std::string lvl = scene->GetLevelId();
        std::wstring wlvl(lvl.begin(), lvl.end());

        swprintf_s(textScore, L"SCORE\n%06d", scene->GetScore());
        swprintf_s(textCoins, L"COINS\n%02d", scene->GetCoins());
        swprintf_s(textWorld, L"WORLD\n%s", wlvl.c_str());
        swprintf_s(textTime, L"TIME\n%03d", scene->GetTime());
        swprintf_s(textLives, L"LIVES\n%d", CMario::lives);

        int colWidth = g->GetBackBufferWidth() / 5;
        int yOffset = 15;
        for (int i = 0; i < 5; ++i)
        {
            RECT rect;
            rect.left = i * colWidth;
            rect.right = (i + 1) * colWidth;
            rect.top = yOffset;
            rect.bottom = yOffset + 80;
            wchar_t *textPtr = nullptr;
            switch (i)
            {
            case 0:
                textPtr = textScore;
                break;
            case 1:
                textPtr = textCoins;
                break;
            case 2:
                textPtr = textWorld;
                break;
            case 3:
                textPtr = textTime;
                break;
            case 4:
                textPtr = textLives;
                break;
            }
            g->DrawTextRaw(textPtr, rect, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
        }
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
            if (msg.message == WM_QUIT)
                done = 1;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        time.Tick();

        while (time.ShouldUpdate())
        {
            Update(time.GetDeltaTime() * 1000.0f);
            time.OnUpdate();
            if (CGame::GetInstance()->IsExitLevelRequested())
                done = 1;
        }

        Render();
        time.OnRender();

        time.PrintFPS();
    }

    CSceneManager::GetInstance()->SetScene(nullptr);
    scene = nullptr;

    return 1;
}