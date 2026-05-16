#include "CResourceRegistry.h"
#include "../../engine/rendering/Textures.h"
#include "../../engine/Graphics/Sprites.h"
#include "../../engine/Graphics/Animations.h"
#include "../../engine/audio/CAudioManager.h"
#include "../../engine/utils/debug.h"
#include <fstream>
#include <sstream>

CResourceRegistry* CResourceRegistry::__instance = NULL;

CResourceRegistry* CResourceRegistry::GetInstance() {
    if (__instance == NULL) __instance = new CResourceRegistry();
    return __instance;
}

void CResourceRegistry::RegisterName(std::string name, int id) {
    nameToId[name] = id;
}

int CResourceRegistry::GetID(std::string name) {
    if (nameToId.find(name) == nameToId.end()) {
        DebugOut(L"[ERROR] Resource name %hs not found in Registry!\n", name.c_str());
        return -1;
    }
    return nameToId[name];
}

void CResourceRegistry::AddSprite(int id, std::string name, int l, int t, int r, int b, int textureId) {
    auto tex = CTextures::GetInstance()->Get(textureId);
    if (!tex) {
        DebugOut(L"[ERROR] Texture ID %d not found for sprite %hs\n", textureId, name.c_str());
        return;
    }
    CSprites::GetInstance()->Add(id, l, t, r, b, tex);
    RegisterName(name, id);
}

void CResourceRegistry::AddAnimation(int id, std::string name, int defaultTime, std::initializer_list<int> spriteIds) {
    LPANIMATION ani = new CAnimation(defaultTime);
    for (int spriteId : spriteIds) {
        ani->Add(spriteId);
    }
    CAnimations::GetInstance()->Add(name, id, ani);
    RegisterName(name, id);
}

void CResourceRegistry::RenderAnimation(std::string name, float x, float y, int nx) {
    int id = GetID(name);
    if (id != -1) {
        CAnimations::GetInstance()->Get(id)->Render(x, y, nx);
    }
}

void CResourceRegistry::LoadResourcesForPlayScene() {
    // 1. Load Textures
    std::ifstream texFile("content/configs/textures.txt");
    if (!texFile.is_open()) {
        DebugOut(L"[ERROR] Failed to open content/configs/textures.txt\n");
    } else {
        DebugOut(L"[INFO] Opened textures.txt\n");
        std::string line;
        int count = 0;
        while (std::getline(texFile, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::stringstream ss(line);
            int id; std::string path;
            ss >> id >> path;
            std::wstring wpath(path.begin(), path.end());
            CTextures::GetInstance()->Add(id, wpath.c_str());
            DebugOut(L"[INFO] Loaded Texture ID %d from %hs\n", id, path.c_str());
            count++;
        }
        DebugOut(L"[INFO] Total Textures loaded: %d\n", count);
    }

    // 2. Load Sprites
    std::ifstream sprFile("content/configs/sprites.txt");
    if (!sprFile.is_open()) {
        DebugOut(L"[ERROR] Failed to open content/configs/sprites.txt\n");
    } else {
        std::string line;
        while (std::getline(sprFile, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::stringstream ss(line);
            int id, l, t, r, b, texId; std::string name;
            ss >> id >> name >> l >> t >> r >> b >> texId;
            AddSprite(id, name, l, t, r, b, texId);
        }
        DebugOut(L"[INFO] Sprites loaded from config.\n");
    }

    // 3. Load Animations
    std::ifstream aniFile("content/configs/animations.txt");
    if (!aniFile.is_open()) {
        DebugOut(L"[ERROR] Failed to open content/configs/animations.txt\n");
    } else {
        std::string line;
        while (std::getline(aniFile, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::stringstream ss(line);
            int id, aniTime; std::string name;
            ss >> id >> name >> aniTime;
            LPANIMATION ani = new CAnimation(aniTime);
            int spriteId;
            while (ss >> spriteId) {
                ani->Add(spriteId);
            }
            CAnimations::GetInstance()->Add(name, id, ani);
            RegisterName(name, id);
        }
        DebugOut(L"[INFO] Animations loaded from config.\n");
    }

    // 4. Load Sounds
    CAudioManager* audio = CAudioManager::GetInstance();
    audio->LoadSound("jump", "content/audio/jump-small.wav");
    audio->LoadSound("coin", "content/audio/coin.wav");
    audio->LoadSound("break", "content/audio/breakblock.wav");
    audio->LoadSound("die", "content/audio/mariodie.wav");
    audio->LoadSound("powerup", "content/audio/powerup.wav");
    audio->LoadSound("stomp", "content/audio/stomp.wav");
    audio->LoadSound("kick", "content/audio/kick.wav");
    audio->LoadSound("bump", "content/audio/bump.wav");
    audio->LoadSound("fireball", "content/audio/fireball.wav");
    audio->LoadSound("pipe", "content/audio/pipe.wav");
    audio->LoadSound("stage_clear", "content/audio/stage_clear.wav");

    DebugOut(L"[INFO] PlayScene resources loading complete.\n");
}

void CResourceRegistry::Clear() {
    nameToId.clear();
}
