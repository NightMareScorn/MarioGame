#include "CSceneManager.h"

CSceneManager* CSceneManager::instance = nullptr;

CSceneManager* CSceneManager::GetInstance() {
    if (!instance) instance = new CSceneManager();
    return instance;
}

void CSceneManager::SetScene(CScene* scene) {
    if (currentScene) currentScene->Unload();
    currentScene = scene;
    if (currentScene) currentScene->Load();
}

void CSceneManager::Update(float dt) {
    if (currentScene) currentScene->Update(dt);
}

void CSceneManager::Render() {
    if (currentScene) currentScene->Render();
}
