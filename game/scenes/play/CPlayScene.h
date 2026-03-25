#pragma once
#include "../CScene.h"
#include "../../entities/player/CMario.h"
#include "../../entities/blocks/CBlock.h"
#include <vector>

class CPlayScene : public CScene {
    CMario* mario;
    std::vector<CBlock*> blocks;

public:
    void Load() override;
    void Update(float dt) override;
    void Render() override;
    void Unload() override;
};
