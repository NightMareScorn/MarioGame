#include "CResourceRegistry.h"
#include "../../engine/rendering/Textures.h"
#include "../../engine/Graphics/Sprites.h"
#include "../../engine/Graphics/Animations.h"
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
    CAnimations::GetInstance()->Add(id, ani);
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
    // Format: [ID] [Path]
    std::ifstream texFile("content/configs/textures.txt");
    std::string line;
    while (std::getline(texFile, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        int id; std::string path;
        ss >> id >> path;
        std::wstring wpath(path.begin(), path.end());
        CTextures::GetInstance()->Add(id, wpath.c_str());
    }

    // 2. Load Sprites
    // Format: [ID] [Name] [L] [T] [R] [B] [TexID]
    std::ifstream sprFile("content/configs/sprites.txt");
    while (std::getline(sprFile, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        int id, l, t, r, b, texId; std::string name;
        ss >> id >> name >> l >> t >> r >> b >> texId;
        AddSprite(id, name, l, t, r, b, texId);
    }

    // 3. Load Animations
    // Format: [ID] [Name] [Time] [SpriteID1] [SpriteID2] ...
    std::ifstream aniFile("content/configs/animations.txt");
    while (std::getline(aniFile, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        int id, time; std::string name;
        ss >> id >> name >> time;
        LPANIMATION ani = new CAnimation(time);
        int spriteId;
        while (ss >> spriteId) {
            ani->Add(spriteId);
        }
        CAnimations::GetInstance()->Add(id, ani);
        RegisterName(name, id);
    }

    DebugOut(L"[INFO] PlayScene resources loaded via Data Files.\n");
}

void CResourceRegistry::Clear() {
    nameToId.clear();
}
