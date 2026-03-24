#include "Camera.h"
#include "../core/Game.h"

CCamera* CCamera::__instance = NULL;

CCamera* CCamera::GetInstance()
{
	if (__instance == NULL) __instance = new CCamera();
	return __instance;
}

void CCamera::Update(float targetX, float targetY, DWORD dt)
{
	// Simple interpolation for smooth movement
	// cam_x = targetX - (screenWidth / 2)
	// cam_y = targetY - (screenHeight / 2)
	
	CGame* g = CGame::GetInstance();
	float screenWidth = (float)g->GetBackBufferWidth();
	float screenHeight = (float)g->GetBackBufferHeight();

	float destX = targetX - screenWidth / 2.0f;
	float destY = targetY - screenHeight / 2.0f;

	// Clamp or handle world boundaries if needed (not requested yet)
	
	// Interpolation (lerp)
	float lerpFactor = 0.1f; // Adjust for smoothness
	cam_x += (destX - cam_x) * lerpFactor;
	cam_y += (destY - cam_y) * lerpFactor;
}
