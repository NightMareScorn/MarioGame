#pragma once
#include <string>
#include <vector>
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
    float mapWidth = 0.0f;

public:
    CPlayScene(std::string levelPath = "content/levels/level_1_1.csv") : levelPath(levelPath) {}
    void Load() override;
    void Update(float dt) override;
    void Render() override;
    void Unload() override;
};
