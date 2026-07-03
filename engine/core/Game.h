#pragma once
#include <Windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include "Texture.h"

#include "Camera.h"
class CGame
{
	static CGame *__instance;
	HWND hWnd;

	int backBufferWidth = 0;
	int backBufferHeight = 0;

	int minWidth = 800;
	int minHeight = 600;

	float renderOffsetX = 0;
	float renderOffsetY = 0;
	float renderScale = 1.0f;

	// Viewport = visible world area. Smaller values = more zoom.
	// NES resolution: 256x240. Window: 640x480 → ~2.5x zoom.
	int viewportWidth = 256;
	int viewportHeight = 240;

	ID3D10Device *pD3DDevice = NULL;
	IDXGISwapChain *pSwapChain = NULL;
	ID3D10RenderTargetView *pRenderTargetView = NULL;
	ID3D10BlendState *pBlendStateAlpha = NULL;

	LPD3DX10SPRITE spriteObject;

	HINSTANCE hInstance;

	ID3D10SamplerState *pPointSamplerState;
	ID3DX10Font *pFont = NULL;

	bool exitLevelRequested = false;

public:
	void Init(HWND hWnd, HINSTANCE hInstance);

	void Draw(float x, float y, LPTEXTURE tex, RECT *rect = NULL, float alpha = 1.0f, int sprite_width = 0, int sprite_height = 0);
	void DrawTextRaw(LPCWSTR text, RECT rect, D3DXCOLOR color);
	void Draw(float x, float y, LPTEXTURE tex, int l, int t, int r, int b, float alpha = 1.0f, int sprite_width = 0, int sprite_height = 0)
	{
		RECT rect;
		rect.left = l;
		rect.top = t;
		rect.right = r;
		rect.bottom = b;
		this->Draw(x, y, tex, &rect, alpha, sprite_width, sprite_height);
	}

	LPTEXTURE LoadTexture(LPCWSTR texturePath);

	ID3D10Device *GetDirect3DDevice() { return this->pD3DDevice; }
	IDXGISwapChain *GetSwapChain() { return this->pSwapChain; }
	ID3D10RenderTargetView *GetRenderTargetView() { return this->pRenderTargetView; }
	ID3DX10Sprite *GetSpriteHandler() { return this->spriteObject; }
	ID3D10BlendState *GetAlphaBlending() { return pBlendStateAlpha; };

	int GetBackBufferWidth() { return backBufferWidth; }
	int GetBackBufferHeight() { return backBufferHeight; }

	// Viewport = the visible world area (for zoom)
	int GetViewportWidth() { return viewportWidth; }
	int GetViewportHeight() { return viewportHeight; }

	float GetRenderOffsetX() { return renderOffsetX; }
	float GetRenderOffsetY() { return renderOffsetY; }
	float GetRenderScale() { return renderScale; }

	static CGame *GetInstance();

	void ProcessResize(int width, int height);
	void SetPointSamplerState();

	void GetCamPos(float &x, float &y) { CCamera::GetInstance()->GetCamPos(x, y); }
	void SetCamPos(float x, float y) { CCamera::GetInstance()->SetCamPos(x, y); }

	void SetExitLevel(bool exit) { exitLevelRequested = exit; }
	bool IsExitLevelRequested() { return exitLevelRequested; }

	~CGame();
};
typedef CGame *LPGAME;
