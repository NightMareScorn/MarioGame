#include "Camera.h"
#include "../core/Game.h"
#include <cmath>

CCamera* CCamera::__instance = NULL;

CCamera* CCamera::GetInstance()
{
	if (__instance == NULL) __instance = new CCamera();
	return __instance;
}

void CCamera::Update(float targetX, float targetY, DWORD dt)
{
	CGame* g = CGame::GetInstance();
	float screenWidth = (float)g->GetViewportWidth();
	float screenHeight = (float)g->GetViewportHeight();

	float destX = targetX - screenWidth / 2.0f;
	float destY = targetY - screenHeight / 2.0f;

	// In typical SMB, the camera does not go below 0 so the ground sits exactly at the bottom of the screen.
	if (destY < 0.0f) destY = 0.0f;

	// Better interpolation for smoother movement
	float lerpFactor = 0.05f; // Slower follow for "weight"
	
	// Optional: Deadzone (target must move 5px before camera starts following)
	if (fabsf(destX - cam_x) > 5.0f) {
		cam_x += (destX - cam_x) * lerpFactor;
	}
	
	if (fabsf(destY - cam_y) > 5.0f) {
		cam_y += (destY - cam_y) * lerpFactor;
	}
}
