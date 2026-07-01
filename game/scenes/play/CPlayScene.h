#pragma once
#include <string>
#include <vector>
#include <cmath>
#include "../CScene.h"
#include "../../entities/player/CMario.h"
#include "../../entities/blocks/CBlock.h"

class CPlayScene : public CScene {
    friend class CMapLoader;
    std::string levelPath;
    CMario* mario;
    std::vector<CBlock*> blocks;
    std::vector<CGameObject*> decors;
    std::vector<CGameObject*> enemies;
    std::vector<CGameObject*> items;
    std::vector<CGameObject*> foregrounds;
    float mapWidth = 0.0f;
    int bgR = 92, bgG = 148, bgB = 252; // Default Mario blue (#5C94FC)

    std::string currentMapPath;
    std::string pendingMapPath;

public:
    CPlayScene(std::string levelPath = "content/levels/level_1_1.csv") : levelPath(levelPath) {}
    void Load() override;
    void Load(const std::string& mapPath);
    void Update(float dt) override;
    void Render() override;
    void Unload() override;

    void SetBackgroundColor(int r, int g, int b) { bgR = r; bgG = g; bgB = b; }
    void GetBackgroundColor(int& r, int& g, int& b) { r = bgR; g = bgG; b = bgB; }
    void SetClearColor(D3DXCOLOR color) {
        bgR = (int)(color.r * 255);
        bgG = (int)(color.g * 255);
        bgB = (int)(color.b * 255);
    }
    D3DXCOLOR GetClearColor() {
        return D3DXCOLOR(bgR / 255.0f, bgG / 255.0f, bgB / 255.0f, 1.0f);
    }

    void TransitionToMap(const std::string& mapPath) { pendingMapPath = mapPath; }

    // Lọc các đối tượng trong khoảng 256 pixels quanh (x, y)
    template <typename T>
    std::vector<CGameObject*>
    GetObjectsInRange(float x, float y, const std::vector<T*>& sourceList)
    {
        std::vector<CGameObject*> result;
        for (auto obj : sourceList)
            if (std::abs(obj->x - x) < 256.0f && std::abs(obj->y - y) < 256.0f)
                result.push_back(obj);
        return result;
    }
};
