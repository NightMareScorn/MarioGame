#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <map>

struct SoundBuffer {
    char* data;
    size_t size;
};

class CAudioManager {
    static CAudioManager* __instance;
    std::map<std::string, SoundBuffer> soundMap;
public:
    ~CAudioManager();
    static CAudioManager* GetInstance();
    void LoadSound(std::string name, std::string path);
    void Play(std::string name, bool loop = false);
    void Stop();
};
