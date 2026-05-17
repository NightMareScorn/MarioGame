#include "Game.h"
#include "debug.h"

CGame * CGame::__instance = NULL;

void CGame::Init(HWND hWnd, HINSTANCE hInstance)
{
	this->hWnd = hWnd;
	this->hInstance = hInstance;

	RECT r;
	GetClientRect(hWnd, &r);

	backBufferWidth = r.right + 1;
	backBufferHeight = r.bottom + 1;

	DebugOut(L"[INFO] Window's client area: width= %d, height= %d\n", r.right - 1, r.bottom - 1);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = backBufferWidth;
	swapChainDesc.BufferDesc.Height = backBufferHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;

	HRESULT hr = D3D10CreateDeviceAndSwapChain(NULL,
		D3D10_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		D3D10_SDK_VERSION,
		&swapChainDesc,
		&pSwapChain,
		&pD3DDevice);

	if (hr != S_OK)
	{
		DebugOut((wchar_t*)L"[INFO] D3D10_DRIVER_TYPE_HARDWARE failed, trying WARP... \n");
		hr = D3D10CreateDeviceAndSwapChain(NULL,
			D3D10_DRIVER_TYPE_WARP,
			NULL,
			0,
			D3D10_SDK_VERSION,
			&swapChainDesc,
			&pSwapChain,
			&pD3DDevice);
	}

	if (hr != S_OK)
	{
		DebugOut((wchar_t*)L"[ERROR] D3D10CreateDeviceAndSwapChain has failed %s %d", _W(__FILE__), __LINE__);
		return;
	}

	ID3D10Texture2D* pBackBuffer;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	if (hr != S_OK)
	{
		DebugOut((wchar_t*)L"[ERROR] pSwapChain->GetBuffer has failed %s %d", _W(__FILE__), __LINE__);
		return;
	}

	hr = pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);

	pBackBuffer->Release();
	if (hr != S_OK)
	{
		DebugOut((wchar_t*)L"[ERROR] CreateRenderTargetView has failed %s %d", _W(__FILE__), __LINE__);
		return;
	}

	pD3DDevice->OMSetRenderTargets(1, &pRenderTargetView, NULL);

	D3D10_VIEWPORT viewPort;
	viewPort.Width = backBufferWidth;
	viewPort.Height = backBufferHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	pD3DDevice->RSSetViewports(1, &viewPort);

	D3D10_BLEND_DESC StateDesc;
	ZeroMemory(&StateDesc, sizeof(D3D10_BLEND_DESC));
	StateDesc.AlphaToCoverageEnable = FALSE;
	StateDesc.BlendEnable[0] = TRUE;
	StateDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	StateDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	StateDesc.BlendOp = D3D10_BLEND_OP_ADD;
	StateDesc.SrcBlendAlpha = D3D10_BLEND_ZERO;
	StateDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	StateDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	StateDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

	pD3DDevice->CreateBlendState(&StateDesc, &pBlendStateAlpha);

	D3DX10CreateSprite(pD3DDevice, 0, &spriteObject);

    D3DX10_FONT_DESCW fd;
    ZeroMemory(&fd, sizeof(fd));
    fd.Height = 16;
    fd.Width = 0;
    fd.Weight = FW_BOLD;
    fd.MipLevels = 1;
    fd.Italic = false;
    fd.CharSet = DEFAULT_CHARSET;
    fd.OutputPrecision = OUT_DEFAULT_PRECIS;
    fd.Quality = ANTIALIASED_QUALITY;
    fd.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    wcscpy_s(fd.FaceName, L"Arial");
    D3DX10CreateFontIndirectW(pD3DDevice, &fd, &pFont);

    SetPointSamplerState();

    DebugOut(L"[INFO] InitDirectX has been successful\n");
}

void CGame::SetPointSamplerState()
{
	D3D10_SAMPLER_DESC desc;
	desc.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT;
	desc.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
	desc.MipLODBias = 0;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D10_COMPARISON_NEVER;
	desc.BorderColor[0] = 1.0f;
	desc.BorderColor[1] = 1.0f;
	desc.BorderColor[2] = 1.0f;
	desc.BorderColor[3] = 1.0f;
	desc.MinLOD = -D3D10_FLOAT32_MAX;
	desc.MaxLOD = D3D10_FLOAT32_MAX;

	pD3DDevice->CreateSamplerState(&desc, &pPointSamplerState);
}
void CGame::DrawTextRaw(LPCWSTR text, RECT rect, D3DXCOLOR color)
{
    if (pFont == NULL) return;
    pFont->DrawTextW(NULL, text, -1, &rect, DT_LEFT | DT_TOP | DT_NOCLIP, color);
}
void CGame::Draw(float x, float y, LPTEXTURE tex, RECT* rect, float alpha, int sprite_width, int sprite_height)
{
	if (tex == NULL) return;
	
	ID3D10ShaderResourceView* pRsView = tex->getShaderResourceView();
	if (pRsView == NULL) return;

	int width = (sprite_width > 0) ? sprite_width : tex->getWidth();
	int height = (sprite_height > 0) ? sprite_height : tex->getHeight();

	D3DXMATRIX matScaling;
	D3DXMATRIX matTranslation;

	D3DXMatrixScaling(&matScaling, (float)width, (float)height, 1.0f);
	D3DXMatrixTranslation(&matTranslation, x, backBufferHeight - y, 0.1f);

	D3DXMATRIX matTransform = matScaling * matTranslation;

	D3DXVECTOR2 spriteCentre = D3DXVECTOR2(width / 2.0f, height / 2.0f);

	D3DX10_SPRITE sprite;
	sprite.pTexture = pRsView;
	sprite.TexCoord.x = 0;
	sprite.TexCoord.y = 0;
	sprite.TexSize.x = 1.0f;
	sprite.TexSize.y = 1.0f;
	sprite.ColorModulate = D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha);
	sprite.matWorld = matTransform;
	sprite.TextureIndex = 0;

	if (rect != NULL)
	{
		sprite.TexCoord.x = rect->left / (float)tex->getWidth();
		sprite.TexCoord.y = rect->top / (float)tex->getHeight();
		sprite.TexSize.x = (rect->right - rect->left) / (float)tex->getWidth();
		sprite.TexSize.y = (rect->bottom - rect->top) / (float)tex->getHeight();
	}

	D3DXMATRIX matOrtho;
	D3DXMatrixOrthoOffCenterLH(&matOrtho, 0, (float)backBufferWidth, 0, (float)backBufferHeight, 0.1f, 10.0f);
	spriteObject->SetProjectionTransform(&matOrtho);

	spriteObject->DrawSpritesBuffered(&sprite, 1);
}

LPTEXTURE CGame::LoadTexture(LPCWSTR texturePath)
{
	ID3D10Resource* pD3D10Resource = NULL;
	ID3D10ShaderResourceView* pShaderResourceView = NULL;

	HRESULT hr = D3DX10CreateTextureFromFile(pD3DDevice,
		texturePath,
		NULL,
		NULL,
		&pD3D10Resource,
		NULL);

	if (FAILED(hr))
	{
		DebugOut((wchar_t*)L"[ERROR] CreateTextureFromFile failed %s\n", texturePath);
		return NULL;
	}

	pD3D10Resource->QueryInterface(__uuidof(ID3D10Texture2D), (LPVOID*)&pD3D10Resource);

	hr = pD3DDevice->CreateShaderResourceView(pD3D10Resource, NULL, &pShaderResourceView);

	if (FAILED(hr))
	{
		DebugOut((wchar_t*)L"[ERROR] CreateShaderResourceView failed %s\n", texturePath);
		return NULL;
	}

	return new CTexture((ID3D10Texture2D*)pD3D10Resource, pShaderResourceView);
}

CGame* CGame::GetInstance()
{
	if (__instance == NULL) __instance = new CGame();
	return __instance;
}

CGame::~CGame()
{    if (pFont != NULL) pFont->Release();	if (pPointSamplerState != NULL) pPointSamplerState->Release();
	if (spriteObject != NULL) spriteObject->Release();
	if (pBlendStateAlpha != NULL) pBlendStateAlpha->Release();
	if (pRenderTargetView != NULL) pRenderTargetView->Release();
	if (pSwapChain != NULL) pSwapChain->Release();
	if (pD3DDevice != NULL) pD3DDevice->Release();
}
