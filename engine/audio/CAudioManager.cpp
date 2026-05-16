#include "CAudioManager.h"
#include "../utils/debug.h"
#pragma comment(lib, "winmm.lib")

#include <fstream>

CAudioManager* CAudioManager::__instance = NULL;

CAudioManager::~CAudioManager() {
    for (auto const& [name, buffer] : soundMap) {
        delete[] buffer.data;
    }
}

CAudioManager* CAudioManager::GetInstance() {
    if (__instance == NULL) __instance = new CAudioManager();
    return __instance;
}

void CAudioManager::LoadSound(std::string name, std::string path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        DebugOut(L"[ERROR] Failed to open sound file: %hs\n", path.c_str());
        return;
    }

    size_t size = file.tellg();
    char* data = new char[size];
    file.seekg(0, std::ios::beg);
    file.read(data, size);
    file.close();

    // Clean up old buffer if exists
    if (soundMap.find(name) != soundMap.end()) {
        delete[] soundMap[name].data;
    }

    soundMap[name] = { data, size };
    DebugOut(L"[INFO] Sound cached to RAM: %hs (%zu bytes)\n", name.c_str(), size);
}

void CAudioManager::Play(std::string name, bool loop) {
    if (soundMap.find(name) != soundMap.end()) {
        DWORD flags = SND_MEMORY | SND_ASYNC;
        if (loop) flags |= SND_LOOP;
        PlaySoundA((LPCSTR)soundMap[name].data, NULL, flags);
    } else {
        DebugOut(L"[ERROR] Sound %hs not found in AudioManager!\n", name.c_str());
    }
}

void CAudioManager::Stop() {
    PlaySound(NULL, NULL, 0);
}
