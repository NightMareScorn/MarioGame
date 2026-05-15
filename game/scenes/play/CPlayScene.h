#pragma once
#include "../CScene.h"
#include "../../entities/player/CMario.h"
#include "../../entities/blocks/CBlock.h"
#include <vector>
#include <string>

class CPlayScene : public CScene {
    friend class CMapLoader;
    CMario* mario;
    std::vector<CBlock*> blocks;
    std::vector<CGameObject*> decors; // Background decorations (no collision)
    std::vector<CGameObject*> enemies;
    std::vector<CGameObject*> items;

    std::string currentMapPath;
    std::string pendingMapPath;

public:
    void Load() override { Load("content/levels/level_1_1.csv"); }
    void Load(std::string mapPath);
    void Update(float dt) override;
    void Render() override;
    void Unload() override;

    void TransitionToMap(std::string mapPath) { pendingMapPath = mapPath; }
    void SetClearColor(D3DXCOLOR color) { clearColor = color; }
};
