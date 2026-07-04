#pragma once
#include <Windows.h>

class CCamera
{
	static CCamera* __instance;
	float cam_x = 0.0f;
	float cam_y = 0.0f;
	float mapWidth = 0.0f;

public:
	static CCamera* GetInstance();

	void SetMapWidth(float w) { mapWidth = w; }
	void SetCamPos(float x, float y) { cam_x = x; cam_y = y; }
	void GetCamPos(float& x, float& y) { x = cam_x; y = cam_y; }

	float GetCamX() { return cam_x; }
	float GetCamY() { return cam_y; }

	void Update(float targetX, float targetY, DWORD dt);

	~CCamera() {}
};
