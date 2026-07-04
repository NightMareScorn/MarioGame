#pragma once
#include <string>
#include <vector>
#include <cmath>
#include "../CScene.h"
#include "../../entities/player/CMario.h"
#include "../../entities/blocks/CBlock.h"

enum class ECastleClearState
{
    NONE,
    BRIDGE_COLLAPSING,
    BOWSER_FALLING,
    MARIO_WALKING_TO_TOAD,
    TOAD_TALKING,
    FINISHED
};

class CPlayScene : public CScene
{
    friend class CMapLoader;
    friend class CAxe;
    std::string levelPath;
    CMario *mario;
    std::vector<CBlock *> blocks;
    std::vector<CGameObject *> decors;
    std::vector<CGameObject *> enemies;
    std::vector<CGameObject *> pendingEnemies;
    std::vector<CGameObject *> items;
    std::vector<CGameObject *> foregrounds;
    float mapWidth = 0.0f;
    std::string theme = "surface";
    int bgR = 92, bgG = 148, bgB = 252; // Default Mario blue (#5C94FC)

    std::string currentMapPath;
    std::string pendingMapPath;
    float goalTimer = 0.0f;

    // HUD stats
    int score = 0;
    int coins = 0;
    float timeLeft = 400.0f;

    // Pause game
    bool isPaused = false;
    int pauseSelection = 0; // 0: Mute, 1: Back to Menu

    // Game over
    bool isGameOver = false;

    // Castle clear animation state
    ECastleClearState castleClearState = ECastleClearState::NONE;
    float castleClearTimer = 0.0f;
    std::vector<class CBridge *> bridgeBlocks;
    size_t nextBridgeBlockIndex = 0;
    class CDecorBlock *toadNPC = nullptr;
    float toadTextTimer = 0.0f;

public:
    CPlayScene::CPlayScene(std::string levelPath) : levelPath(levelPath)
    {
        CMario::lives = 3;
        CMario::currentPower = EMarioPower::SMALL;
        CMario::hasCheckpoint = false;
        CMario::checkpointX = 0.0f;
        CMario::checkpointY = 0.0f;
    }
    void Load() override;
    void Load(const std::string &mapPath);
    void Update(float dt) override;
    void Render() override;
    void Unload() override;

    void SetBackgroundColor(int r, int g, int b)
    {
        bgR = r;
        bgG = g;
        bgB = b;
    }
    void GetBackgroundColor(int &r, int &g, int &b)
    {
        r = bgR;
        g = bgG;
        b = bgB;
    }
    void SetClearColor(D3DXCOLOR color)
    {
        bgR = (int)(color.r * 255);
        bgG = (int)(color.g * 255);
        bgB = (int)(color.b * 255);
    }
    D3DXCOLOR GetClearColor()
    {
        return D3DXCOLOR(bgR / 255.0f, bgG / 255.0f, bgB / 255.0f, 1.0f);
    }

    void AddItem(CGameObject *item) { items.push_back(item); }
    void AddEnemy(CGameObject *enemy) { pendingEnemies.push_back(enemy); }
    std::string GetCurrentMapPath() const { return currentMapPath; }
    ECastleClearState GetCastleClearState() const { return castleClearState; }
    CMario *GetPlayer() { return mario; }
    std::string GetTheme() const { return theme; }

    int GetScore() const { return score; }
    void AddScore(int s) { score += s; }
    int GetCoins() const { return coins; }
    void AddCoin() { coins++; }
    int GetTime() const { return (int)std::ceil(timeLeft); }
    std::string GetLevelId() const
    {
        std::string levelId = "1-1";
        size_t lastSlash = levelPath.find_last_of("\\/");
        std::string fileName = (lastSlash == std::string::npos) ? levelPath : levelPath.substr(lastSlash + 1);
        if (fileName.find("level_") == 0)
        {
            levelId = fileName.substr(6, 3);
            for (size_t i = 0; i < levelId.length(); ++i)
            {
                if (levelId[i] == '_')
                    levelId[i] = '-';
            }
        }
        return levelId;
    }

    void TransitionToMap(const std::string &mapPath) { pendingMapPath = mapPath; }

    // Lọc các đối tượng trong khoảng 256 pixels quanh (x, y)
    template <typename T>
    std::vector<CGameObject *>
    GetObjectsInRange(float x, float y, const std::vector<T *> &sourceList)
    {
        std::vector<CGameObject *> result;
        for (auto obj : sourceList)
            if (std::abs(obj->x - x) < 256.0f && std::abs(obj->y - y) < 256.0f)
                result.push_back(obj);
        return result;
    }
};

extern CPlayScene *scene;
