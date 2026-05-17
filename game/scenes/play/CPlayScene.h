#pragma once
#include "../../entities/blocks/CBlock.h"
#include "../../entities/player/CMario.h"
#include "../CScene.h"
#include <cmath>
#include <string>
#include <vector>


class CPlayScene : public CScene {
  friend class CMapLoader;
  CMario *mario;
  std::vector<CBlock *> blocks;
  std::vector<CGameObject *> decors; // Background
  std::vector<CGameObject *> enemies;
  std::vector<CGameObject *> items;

  std::string currentMapPath;
  std::string pendingMapPath;

  // HUD stats
  int score = 0;
  int coins = 0;
  float timeLeft = 400.0f;

public:
  void Load() override {
    Load("content/levels/level_1_1.csv");  // TODO: Remove hardcoded path
  }
  void Load(const std::string &mapPath);
  void Update(float dt) override;
  void Render() override;
  void Unload() override;

  int GetScore() const { return score; }
  void AddScore(int s) { score += s; }
  int GetCoins() const { return coins; }
  void AddCoin() { coins++; }
  int GetTime() const { return (int)std::ceil(timeLeft); }

  void TransitionToMap(const std::string &mapPath) { pendingMapPath = mapPath; }
  void SetClearColor(D3DXCOLOR color) { clearColor = color; }

  // Lọc các đối tượng trong khoảng 256 pixels quanh (x, y)
  template <typename T>
  std::vector<CGameObject *>
  GetObjectsInRange(float x, float y, const std::vector<T *> &sourceList) {
    std::vector<CGameObject *> result;
    for (auto obj : sourceList)
      if (std::abs(obj->x - x) < 256.0f && std::abs(obj->y - y) < 256.0f)
        result.push_back(obj);
    return result;
  }
};

extern CPlayScene* scene;