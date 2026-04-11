#pragma once
#include "../CScene.h"
#include "../../entities/player/CMario.h"
#include "../../entities/blocks/CBlock.h"
#include <vector>

class CPlayScene : public CScene {
    friend class CMapLoader;
    CMario* mario;
    std::vector<CBlock*> blocks;
    std::vector<CGameObject*> decors; // Background decorations (no collision)
    std::vector<CGameObject*> enemies;
    std::vector<CGameObject*> items;

public:
    void Load() override;
    void Update(float dt) override;
    void Render() override;
    void Unload() override;
};
