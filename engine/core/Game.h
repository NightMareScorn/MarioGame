#pragma once
#include <Windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include "Texture.h"

#include "Camera.h"
class CGame
{
	static CGame* __instance;
	HWND hWnd;

	int backBufferWidth = 0;
	int backBufferHeight = 0;

	ID3D10Device* pD3DDevice = NULL;
	IDXGISwapChain* pSwapChain = NULL;
	ID3D10RenderTargetView* pRenderTargetView = NULL;
	ID3D10BlendState* pBlendStateAlpha = NULL;

	LPD3DX10SPRITE spriteObject;

	HINSTANCE hInstance;

	ID3D10SamplerState* pPointSamplerState;

public:
	void Init(HWND hWnd, HINSTANCE hInstance);

	void Draw(float x, float y, LPTEXTURE tex, RECT* rect = NULL, float alpha = 1.0f, int sprite_width = 0, int sprite_height = 0);

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

	ID3D10Device* GetDirect3DDevice() { return this->pD3DDevice; }
	IDXGISwapChain* GetSwapChain() { return this->pSwapChain; }
	ID3D10RenderTargetView* GetRenderTargetView() { return this->pRenderTargetView; }
	ID3DX10Sprite* GetSpriteHandler() { return this->spriteObject; }
	ID3D10BlendState* GetAlphaBlending() { return pBlendStateAlpha; };

	int GetBackBufferWidth() { return backBufferWidth; }
	int GetBackBufferHeight() { return backBufferHeight; }

	static CGame* GetInstance();

	void SetPointSamplerState();

	void GetCamPos(float& x, float& y) { CCamera::GetInstance()->GetCamPos(x, y); }
	void SetCamPos(float x, float y) { CCamera::GetInstance()->SetCamPos(x, y); }

	~CGame();
};
typedef CGame* LPGAME;
