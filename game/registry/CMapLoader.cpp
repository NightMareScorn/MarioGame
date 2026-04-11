#include "CMapLoader.h"
#include "CResourceRegistry.h"
#include "../scenes/play/CPlayScene.h"
#include "../entities/blocks/CBrick.h"
#include "../entities/blocks/CLuckyBlock.h"
#include "../entities/blocks/CDecorBlock.h"
#include "../entities/blocks/CInvisibleBlock.h"
#include "../entities/enemies/CGoomba.h"
#include "../entities/enemies/CKoopa.h"
#include "../entities/items/CMushroom.h"
#include "../entities/items/CCoin.h"
#include "../../engine/utils/debug.h"
#include <algorithm>

CMapLoader* CMapLoader::__instance = nullptr;

CMapLoader* CMapLoader::GetInstance() {
    if (__instance == nullptr) __instance = new CMapLoader();
    return __instance;
}

std::string CMapLoader::_Trim(const std::string& s) {
    std::string result = s;
    // Remove trailing \r and \n
    while (!result.empty() && (result.back() == '\r' || result.back() == '\n'))
        result.pop_back();
    // Remove leading/trailing spaces
    size_t start = result.find_first_not_of(' ');
    if (start == std::string::npos) return "";
    size_t end = result.find_last_not_of(' ');
    return result.substr(start, end - start + 1);
}

void CMapLoader::Load(std::string path, CPlayScene* scene) {
    std::ifstream file(path);
    if (!file.is_open()) {
        DebugOut(L"[ERROR] Failed to open map file: %hs\n", path.c_str());
        return;
    }

    DebugOut(L"[INFO] Loading map: %hs\n", path.c_str());

    // Read all lines, trim \r\n
    std::vector<std::string> lines;
    std::string rawLine;
    while (std::getline(file, rawLine)) {
        lines.push_back(_Trim(rawLine));
    }

    std::string currentSection = "";
    std::vector<std::string> tileMapLines;

    for (size_t i = 0; i < lines.size(); i++) {
        const auto& line = lines[i];
        if (line.empty()) continue;

        // Detect section header
        if (line[0] == '#') {
            // If we were accumulating tilemap lines, process them now
            if (currentSection == "# TILEMAP" && !tileMapLines.empty()) {
                _ProcessTileMap(tileMapLines, scene);
                tileMapLines.clear();
            }
            currentSection = line;
            continue;
        }

        // Route to appropriate parser based on current section
        if (currentSection == "# SETTINGS") {
            _ParseSettings(line);
        }
        else if (currentSection == "# TILEMAP") {
            tileMapLines.push_back(line);
        }
        else if (currentSection == "# OBJECTS") {
            _ParseObjectLine(line, scene);
        }
    }

    // Process any remaining tilemap lines (if file ends without another section)
    if (!tileMapLines.empty()) {
        _ProcessTileMap(tileMapLines, scene);
    }

    DebugOut(L"[INFO] Map loading complete.\n");
}

void CMapLoader::_ParseSettings(const std::string& line) {
    auto parts = _Split(line, ',');
    if (parts.size() >= 2 && parts[0] == "cell_size") {
        cellSize = std::stoi(parts[1]);
        DebugOut(L"[INFO] Setting cellSize = %d\n", cellSize);
    }
}

void CMapLoader::_ProcessTileMap(const std::vector<std::string>& lines, CPlayScene* scene) {
    // Parse CSV lines into a 2D matrix of tile IDs
    std::vector<std::vector<int>> matrix;
    for (const auto& line : lines) {
        std::vector<int> row;
        auto cells = _Split(line, ',');
        for (const auto& cell : cells) {
            if (!cell.empty()) row.push_back(std::stoi(cell));
        }
        matrix.push_back(row);
    }

    int H = (int)matrix.size();
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < (int)matrix[i].size(); j++) {
            int id = matrix[i][j];
            float x = (float)(j * cellSize);
            float y = (float)((H - 1 - i) * cellSize); // Y-up: row 0 = top = highest Y

            switch (id) {
                case 0: // Air
                    break;

                case 1: // Ground
                case 5: // Brick (stair/structure)
                case 7: // Brick (platform)
                    scene->blocks.push_back(new CBrick(x, y));
                    break;

                case 4: // Lucky Block (animated ? block)
                    scene->blocks.push_back(new CLuckyBlock(x, y));
                    break;

                case 2: // Cloud - render full 6-part composite cloud
                {
                    // Clouds are precisely cropped in the atlas and have different heights. 
                    // Using mathematically matched pixel offsets to seamlessly stitch them with zero gaps!
                    scene->decors.push_back(new CDecorBlock(x + 0.0f,  y + 0.0f, "ANI_CLOUD_OW_TOP_LEFT"));
                    scene->decors.push_back(new CDecorBlock(x + 8.0f,  y + 0.0f, "ANI_CLOUD_OW_TOP_MID"));
                    scene->decors.push_back(new CDecorBlock(x + 24.0f, y + 0.0f, "ANI_CLOUD_OW_TOP_RIGHT"));
                    scene->decors.push_back(new CDecorBlock(x + 2.0f,  y - 5.0f, "ANI_CLOUD_OW_BOTTOM_LEFT"));
                    scene->decors.push_back(new CDecorBlock(x + 8.0f,  y - 8.0f, "ANI_CLOUD_OW_BOTTOM_MID"));
                    scene->decors.push_back(new CDecorBlock(x + 24.0f, y - 7.0f, "ANI_CLOUD_OW_BOTTOM_RIGHT"));
                    break;
                }

                case 3: // Bush
                    // We don't have bush sprites yet, but mapping is correct per convention.
                    DebugOut(L"[INFO] Bush (ID 3) mapped at col=%d, row=%d but no sprite exists yet.\n", j, i);
                    break;

                case 8: // Flag Pole
                    scene->decors.push_back(new CDecorBlock(x, y, "ANI_FLAG_OW_POLE"));
                    break;

                case 9: // Castle
                    // We don't have Castle sprites yet, using placeholder or skipping.
                    DebugOut(L"[INFO] Castle (ID 9) mapped at col=%d, row=%d but no sprite exists yet.\n", j, i);
                    break;

                default:
                    DebugOut(L"[WARNING] Unknown tile ID %d at col=%d, row=%d\n", id, j, i);
                    break;
            }
        }
    }

    DebugOut(L"[INFO] Parsed TILEMAP: %d blocks, %d decors created.\n",
             (int)scene->blocks.size(), (int)scene->decors.size());
}

void CMapLoader::_ParseObjectLine(const std::string& line, CPlayScene* scene) {
    auto parts = _Split(line, ',');
    if (parts.size() < 3) return;

    std::string type = parts[0];

    // Skip header line
    if (type == "type") return;

    float x = std::stof(parts[1]);
    float y = std::stof(parts[2]);
    // The objects in level_1_1.csv are exported using a Y-down coordinate system for a standard 240px NES screen height.
    // Tiled exports the TOP-LEFT corner of the object. Since most of our entities (Mario, Goomba, Items) are 16x16,
    // we must subtract 16 to correctly find their BOTTOM-LEFT anchor in our Y-up world ecosystem.
    y = 240.0f - y - 16.0f; 

    bool hidden_in_block = false;
    for (size_t i = 3; i < parts.size(); ++i) {
        if (parts[i] == "hidden_in_block=true") hidden_in_block = true;
    }

    CGameObject* spawnedObj = nullptr;

    if (type == "mario") {
        if (scene->mario == nullptr) scene->mario = new CMario();
        scene->mario->x = x;
        scene->mario->y = y;
        DebugOut(L"[INFO] Object: Mario at (%.2f, %.2f)\n", x, y);
    }
    else if (type == "goomba") {
        spawnedObj = new CGoomba(x, y);
        scene->enemies.push_back(spawnedObj);
    }
    else if (type == "koopa") {
        spawnedObj = new CKoopa(x, y);
        scene->enemies.push_back(spawnedObj);
    }
    else if (type == "mushroom" || type == "star" || type == "1-up") { // Treating star/1up as mushroom visually for now
        spawnedObj = new CMushroom(x, y);
        scene->items.push_back(spawnedObj);
    }
    else if (type == "coin") {
        spawnedObj = new CCoin(x, y, hidden_in_block);
        scene->items.push_back(spawnedObj);
    }
    else if (type == "pipe") {
        // Since Y was subtracted by 16 as a general rule, we must add 16 back to get the Top Edge!
        float topEdgeY = y + 16.0f;
        float groundY = 32.0f;
        float pipeHeight = topEdgeY - groundY;
        scene->blocks.push_back(new CInvisibleBlock(x, groundY, 32.0f, pipeHeight)); 
    }

    if (spawnedObj && hidden_in_block) {
        // Móc item vào Lucky Block tại cùng vị trí x, y
        for (auto b : scene->blocks) {
            if (auto lucky = dynamic_cast<CLuckyBlock*>(b)) {
                // Do sai số float, dùng khoảng cách
                if (abs(lucky->x - x) < 2.0f && abs(lucky->y - y) < 2.0f) {
                    lucky->SetHiddenItem(spawnedObj);
                    // Force item state to hidden
                    if (auto m = dynamic_cast<CMushroom*>(spawnedObj)) m->SetState(MUSHROOM_STATE_HIDDEN);
                    if (auto c = dynamic_cast<CCoin*>(spawnedObj)) c->SetState(COIN_STATE_HIDDEN);
                    break;
                }
            }
        }
    }
}

std::vector<std::string> CMapLoader::_Split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        // Clean up any stray whitespace
        token.erase(std::remove(token.begin(), token.end(), '\r'), token.end());
        token.erase(std::remove(token.begin(), token.end(), '\n'), token.end());
        tokens.push_back(token);
    }
    return tokens;
}
