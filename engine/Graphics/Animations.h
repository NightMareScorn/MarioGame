#include <string>
#include <unordered_map>
#include "Animation.h"
#include "Sprite.h"

class CAnimations
{
	static CAnimations* __instance;

	std::unordered_map<int, LPANIMATION> animations;
	std::unordered_map<std::string, int> nameToId;

public:
	void Add(int id, LPANIMATION ani);
	void Add(std::string name, int id, LPANIMATION ani);
	LPANIMATION Get(int id);
	LPANIMATION Get(std::string name);
	void Render(std::string name, float x, float y, int nx = 1);
	void Clear();

	static CAnimations* GetInstance();
};