#pragma once
#include "CScene.h"

class CSceneManager {
    static CSceneManager* instance;
    CScene* currentScene;
    CSceneManager() : currentScene(nullptr) {}

public:
    static CSceneManager* GetInstance();
    void SetScene(CScene* scene);
    CScene* GetCurrentScene() const { return currentScene; }
    void Update(float dt);
    void Render();
};
