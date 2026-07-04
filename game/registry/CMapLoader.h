#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "../../game/entities/CGameObject.h"

class CPlayScene; // Forward declaration

class CMapLoader {
    static CMapLoader* __instance;
    int cellSize = 16;
    float mapHeight = 240.0f; // Default NES height, updated dynamically during tilemap parsing

public:
    static CMapLoader* GetInstance();

    void Load(std::string path, CPlayScene* scene);

private:
    void _ParseSettings(const std::string& line, CPlayScene* scene);
    void _ProcessTileMap(const std::vector<std::string>& lines, CPlayScene* scene);
    void _ParseObjectLine(const std::string& line, CPlayScene* scene);

    std::vector<std::string> _Split(const std::string& s, char delimiter);
    static std::string _Trim(const std::string& s);
};
