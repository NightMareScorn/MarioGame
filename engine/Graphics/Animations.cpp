#include "Animations.h"
#include "../Utils/debug.h"

CAnimations* CAnimations::__instance = NULL;

CAnimations* CAnimations::GetInstance()
{
	if (__instance == NULL) __instance = new CAnimations();
	return __instance;
}

void CAnimations::Add(int id, LPANIMATION ani)
{
	if (animations.find(id) != animations.end())
		DebugOut(L"[WARNING] Animation %d already exists\n", id);

	animations[id] = ani;
}

void CAnimations::Add(std::string name, int id, LPANIMATION ani)
{
	Add(id, ani);
	nameToId[name] = id;
}

LPANIMATION CAnimations::Get(int id)
{
	if (animations.find(id) == animations.end()) {
		DebugOut(L"[ERROR] Animation ID %d not found\n", id);
		return NULL;
	}
	return animations[id];
}

LPANIMATION CAnimations::Get(std::string name)
{
	if (nameToId.find(name) == nameToId.end()) {
		DebugOut(L"[ERROR] Animation name %hs not found\n", name.c_str());
		return NULL;
	}
	return Get(nameToId[name]);
}

void CAnimations::Render(std::string name, float x, float y, int nx)
{
	LPANIMATION ani = Get(name);
	if (ani != NULL)
		ani->Render(x, y, nx);
}

void CAnimations::Clear()
{
	for (auto x : animations)
	{
		LPANIMATION ani = x.second;
		delete ani;
	}

	animations.clear();
	nameToId.clear();
}