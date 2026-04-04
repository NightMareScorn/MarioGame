#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>
#include <initializer_list>

class CResourceRegistry {
    static CResourceRegistry* __instance;
    std::unordered_map<std::string, int> nameToId;
    int nextAutoId = 10000; // IDs above this are auto-assigned

public:
    static CResourceRegistry* GetInstance();

    // Mapping
    void RegisterName(std::string name, int id);
    int GetID(std::string name);

    // Helpers for concise loading
    void AddSprite(int id, std::string name, int l, int t, int r, int b, int textureId);
    void AddAnimation(int id, std::string name, int defaultTime, std::initializer_list<int> spriteIds);

    // Dynamic Rendering
    void RenderAnimation(std::string name, float x, float y, int nx = 1);

    // Scene-specific loading
    void LoadResourcesForPlayScene();

    void Clear();
};
