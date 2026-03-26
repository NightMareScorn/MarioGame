#include "Sprite.h"
#include "Game.h"
#include "Camera.h"

CSprite::CSprite(int id, int left, int top, int right, int bottom, LPTEXTURE tex)
{
	this->id = id;
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
	this->texture = tex;

	float texWidth = (float)tex->getWidth();
	float texHeight = (float)tex->getHeight();

	// Set the sprite’s shader resource view
	sprite.pTexture = tex->getShaderResourceView();

	sprite.TexCoord.x = this->left / texWidth;
	sprite.TexCoord.y = this->top / texHeight;

	int spriteWidth = (this->right - this->left + 1);
	int spriteHeight = (this->bottom - this->top + 1);

	sprite.TexSize.x = spriteWidth / texWidth;
	sprite.TexSize.y = spriteHeight / texHeight;

	sprite.ColorModulate = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	sprite.TextureIndex = 0;

	D3DXMatrixScaling(&this->matScaling, (FLOAT)spriteWidth, (FLOAT)spriteHeight, 1.0f);
}

void CSprite::Draw(float x, float y, int nx)
{
	CCamera* cam = CCamera::GetInstance();
	float cx, cy;
	cam->GetCamPos(cx, cy);

	cx = (FLOAT)floor(cx);
	cy = (FLOAT)floor(cy);

	x = (FLOAT)floor(x);
	y = (FLOAT)floor(y);

	D3DXMATRIX matTranslation;
	CGame* g = CGame::GetInstance();
	D3DXMatrixTranslation(&matTranslation, x - cx, y - cy, 0.1f);

	D3DX10_SPRITE s = this->sprite;
	s.matWorld = (this->matScaling * matTranslation);

	D3DXMATRIX matOrtho;
	D3DXMatrixOrthoOffCenterLH(&matOrtho, 0, (float)g->GetBackBufferWidth(), 0, (float)g->GetBackBufferHeight(), 0.1f, 10.0f);
	g->GetSpriteHandler()->SetProjectionTransform(&matOrtho);

	if (nx < 0)
	{
		float texWidth = (float)texture->getWidth();
		s.TexCoord.x = (this->left + (this->right - this->left + 1)) / texWidth;
		s.TexSize.x = -(this->right - this->left + 1) / texWidth;
	}

	CGame::GetInstance()->GetSpriteHandler()->DrawSpritesBuffered(&s, 1);
}
