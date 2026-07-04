#include "CMapLoader.h"
#include "CResourceRegistry.h"
#include "../scenes/play/CPlayScene.h"
#include "../entities/blocks/CBrick.h"
#include "../entities/blocks/CLuckyBlock.h"
#include "../entities/blocks/CDecorBlock.h"
#include "../entities/blocks/CFlagpole.h"
#include "../entities/blocks/CInvisibleBlock.h"
#include "../entities/blocks/CPipe.h"
#include "../entities/enemies/CGoomba.h"
#include "../entities/enemies/CKoopa.h"
#include "../entities/enemies/CCheepCheep.h"
#include "../entities/enemies/CBowser.h"
#include "../entities/enemies/CFireBar.h"
#include "../entities/enemies/CPodoboo.h"
#include "../entities/enemies/CBlooper.h"
#include "../entities/enemies/CPiranhaPlant.h"
#include "../entities/items/CMushroom.h"
#include "../entities/items/CStar.h"
#include "../entities/items/CFireFlower.h"
#include "../entities/items/CCoin.h"
#include "../entities/blocks/CPlatform.h"
#include "../entities/blocks/CCastleBridge.h"
#include "../entities/blocks/CBridge.h"
#include "../entities/blocks/CLava.h"
#include "../entities/blocks/CAxe.h"
#include "../../engine/utils/debug.h"
#include <algorithm>
#include "../entities/enemies/CBowserFireball.h"

CMapLoader *CMapLoader::__instance = nullptr;

CMapLoader *CMapLoader::GetInstance()
{
    if (__instance == nullptr)
        __instance = new CMapLoader();
    return __instance;
}

std::string CMapLoader::_Trim(const std::string &s)
{
    std::string result = s;
    // Remove trailing \r and \n
    while (!result.empty() && (result.back() == '\r' || result.back() == '\n'))
        result.pop_back();
    // Remove leading/trailing spaces
    size_t start = result.find_first_not_of(' ');
    if (start == std::string::npos)
        return "";
    size_t end = result.find_last_not_of(' ');
    return result.substr(start, end - start + 1);
}

void CMapLoader::Load(std::string path, CPlayScene *scene)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        DebugOut(L"[ERROR] Failed to open map file: %hs\n", path.c_str());
        return;
    }

    DebugOut(L"[INFO] Loading map: %hs\n", path.c_str());

    // Read all lines, trim \r\n
    std::vector<std::string> lines;
    std::string rawLine;
    while (std::getline(file, rawLine))
    {
        lines.push_back(_Trim(rawLine));
    }

    std::string currentSection = "";
    std::vector<std::string> tileMapLines;

    for (size_t i = 0; i < lines.size(); i++)
    {
        const auto &line = lines[i];
        if (line.empty())
            continue;

        // Detect section header
        if (line[0] == '#')
        {
            // If we were accumulating tilemap lines, process them now
            if (currentSection == "# TILEMAP" && !tileMapLines.empty())
            {
                _ProcessTileMap(tileMapLines, scene);
                tileMapLines.clear();
            }
            currentSection = line;
            continue;
        }

        // Route to appropriate parser based on current section
        if (currentSection == "# SETTINGS")
        {
            _ParseSettings(line, scene);
        }
        else if (currentSection == "# TILEMAP")
        {
            tileMapLines.push_back(line);
        }
        else if (currentSection == "# OBJECTS")
        {
            _ParseObjectLine(line, scene);
        }
    }

    // Process any remaining tilemap lines (if file ends without another section)
    if (!tileMapLines.empty())
    {
        _ProcessTileMap(tileMapLines, scene);
    }

    DebugOut(L"[INFO] Map loading complete.\n");
}

void CMapLoader::_ParseSettings(const std::string &line, CPlayScene *scene)
{
    auto parts = _Split(line, ',');
    if (parts.size() < 2)
        return;

    if (parts[0] == "cell_size")
    {
        cellSize = std::stoi(parts[1]);
        DebugOut(L"[INFO] Setting cellSize = %d\n", cellSize);
    }
    else if (parts[0] == "background_color")
    {
        // Support both named colors ("black", "blue") and RGB triplets (r,g,b)
        if (parts.size() >= 4)
        {
            // Numeric RGB format: background_color,r,g,b
            int r = std::stoi(parts[1]);
            int g = std::stoi(parts[2]);
            int b = std::stoi(parts[3]);
            scene->SetBackgroundColor(r, g, b);
            DebugOut(L"[INFO] Setting backgroundColor = (%d, %d, %d)\n", r, g, b);
        }
        else
        {
            // Named color format: background_color,name
            if (parts[1] == "black")
            {
                scene->SetClearColor(D3DXCOLOR(0, 0, 0, 1.0f));
            }
            else if (parts[1] == "blue")
            {
                scene->SetClearColor(D3DXCOLOR(92.0f / 255, 148.0f / 255, 252.0f / 255, 1.0f));
            }
        }
    }
    else if (parts[0] == "theme")
    {
        scene->theme = parts[1];
        DebugOut(L"[INFO] Map theme: %hs\n", parts[1].c_str());
    }
}

void CMapLoader::_ProcessTileMap(const std::vector<std::string> &lines, CPlayScene *scene)
{
    // Parse CSV lines into a 2D matrix of tile IDs
    std::vector<std::vector<int>> matrix;
    for (const auto &line : lines)
    {
        std::vector<int> row;
        auto cells = _Split(line, ',');
        for (const auto &cell : cells)
        {
            if (!cell.empty())
                row.push_back(std::stoi(cell));
        }
        matrix.push_back(row);
    }

    if (!matrix.empty())
    {
        scene->mapWidth = (float)(matrix[0].size() * cellSize);
    }

    int H = (int)matrix.size();
    mapHeight = (float)(H * cellSize);
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < (int)matrix[i].size(); j++)
        {
            int id = matrix[i][j];
            float x = (float)(j * cellSize);
            float y = (float)((H - 1 - i) * cellSize); // Y-up: row 0 = top = highest Y

            switch (id)
            {
            case 0: // Air
                break;

            case 1: // Ground
                scene->blocks.push_back(new CBrick(x, y, "ANI_GROUND_BRICK_OW"));
                break;

            case 5: // Brick (stair/structure)
                scene->blocks.push_back(new CBrick(x, y, "ANI_BRICK_STAIR"));
                break;

            case 6: // Breakable brick
                scene->blocks.push_back(new CBrick(x, y, "ANI_BRICK_IDLE"));
                break;

            case 7: // Brick (platform)
                scene->blocks.push_back(new CBrick(x, y, "ANI_BRICK_PLATFORM"));
                break;

            case 4: // Lucky Block (animated ? block)
                scene->blocks.push_back(new CLuckyBlock(x, y));
                break;

            case 2: // Cloud - render composite cloud (assembled sprite)
            {
                // Clouds are precisely cropped in the atlas and have different heights.
                // Using mathematically matched pixel offsets to seamlessly stitch them with zero gaps!
                CDecorBlock *cloudParts[6];
                cloudParts[0] = new CDecorBlock(x + 0.0f, y + 0.0f, "ANI_CLOUD_OW_TOP_LEFT");
                cloudParts[1] = new CDecorBlock(x + 8.0f, y + 0.0f, "ANI_CLOUD_OW_TOP_MID");
                cloudParts[2] = new CDecorBlock(x + 24.0f, y + 0.0f, "ANI_CLOUD_OW_TOP_RIGHT");
                cloudParts[3] = new CDecorBlock(x + 2.0f, y - 5.0f, "ANI_CLOUD_OW_BOTTOM_LEFT");
                cloudParts[4] = new CDecorBlock(x + 8.0f, y - 8.0f, "ANI_CLOUD_OW_BOTTOM_MID");
                cloudParts[5] = new CDecorBlock(x + 24.0f, y - 7.0f, "ANI_CLOUD_OW_BOTTOM_RIGHT");

                for (int k = 0; k < 6; k++)
                {
                    cloudParts[k]->SetDrifting(true);
                    scene->decors.push_back(cloudParts[k]);
                }
                break;
            }

            case 3: // Bush
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_BUSH_SMALL_ASSEMBLED"));
                break;

            case 8: // Flag Pole segment
            {
                // Kiểm tra: Nếu ô phía DƯỚI ô này KHÔNG PHẢI là số 8 (nghĩa là đây là ô chân cột)
                // thì mới tạo đối tượng CFlagpole có logic.
                bool isBottom = (i == H - 1) || (matrix[i + 1][j] != 8);

                if (isBottom)
                {
                    // Tạo 1 đối tượng duy nhất quản lý toàn bộ cột cờ tại vị trí chân
                    scene->decors.push_back(new CFlagpole(x, y, "ANI_FLAG_OW_POLE"));
                }
                else
                {
                    // Các ô số 8 ở trên chỉ là hình ảnh trang trí, không có logic
                    scene->decors.push_back(new CDecorBlock(x, y, "ANI_FLAG_OW_POLE"));
                }
                break;
            }

            case 9: // Castle
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_3FLOORS_CASTLE_ASSEMBLED"));
                break;

            // --- ASSEMBLED PREFABS ---
            case 10:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_HILL_SMALL_ASSEMBLED"));
                break;
            case 11:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_HILL_BIG_ASSEMBLED"));
                break;
            case 12:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_BUSH_SMALL_ASSEMBLED"));
                break;
            case 13:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_BUSH_BIG_ASSEMBLED"));
                break;
            case 14:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_CLOUD_SMALL_ASSEMBLED"));
                break;
            case 15:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_CLOUD_BIG_ASSEMBLED"));
                break;
            case 16:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_TREE_SMALL_ASSEMBLED"));
                break;
            case 17:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_TREE_BIG_ASSEMBLED"));
                break;
            case 18:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_PIPE_UPWARDS_ASSEMBLED"));
                break;
            case 19:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_2FLOORS_CASTLE_ASSEMBLED"));
                break;
            case 20:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_3FLOORS_CASTLE_ASSEMBLED"));
                break;

            // --- WORLD 2-3 SPECIAL ASSETS ---
            case 21:
            {
                CBridge *bridge = new CBridge(x, y - 8.0f, "ANI_BRIDGE_BLOCK");
                scene->blocks.push_back(bridge);      // Collision (one-way)
                scene->foregrounds.push_back(bridge); // Render over Mario
                break;
            }
            case 22:
                scene->blocks.push_back(new CBrick(x, y, "ANI_MUSHROOM_PLATFORM_LEFT"));
                break;
            case 23:
                scene->blocks.push_back(new CBrick(x, y, "ANI_MUSHROOM_PLATFORM_MID"));
                break;
            case 24:
                scene->blocks.push_back(new CBrick(x, y, "ANI_MUSHROOM_PLATFORM_RIGHT"));
                break;
            case 25:
                scene->blocks.push_back(new CBrick(x, y, "ANI_MUSHROOM_PLATFORM_STEM"));
                break;
            case 26:
                scene->blocks.push_back(new CBrick(x, y, "ANI_PILLAR_BLOCK"));
                break;
            case 27:
                scene->blocks.push_back(new CBrick(x, y, "ANI_LEFT_GREEN_LEAF_BLOCK"));
                break;
            case 28:
                scene->blocks.push_back(new CBrick(x, y, "ANI_MID_GREEN_LEAF_BLOCK"));
                break;
            case 29:
                scene->blocks.push_back(new CBrick(x, y, "ANI_RIGHT_GREEN_LEAF_BLOCK"));
                break;
            case 40:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_WOOD_BLOCK"));
                break;

            // --- WORLD 2-4 SPECIAL ASSETS ---
            case 30:
                scene->blocks.push_back(new CBrick(x, y, "ANI_CASTLE_BRICK_WHITE"));
                break;
            case 31:
                scene->blocks.push_back(new CLava(x, y, "ANI_LAVA_TOP"));
                break;
            case 32:
                scene->blocks.push_back(new CLava(x, y, "ANI_LAVA_BOTTOM"));
                break;
            case 33:
                scene->blocks.push_back(new CBrick(x, y, "ANI_WHITE_FURNACE_BRICK"));
                break;
            case 34:
            {
                CBridge *bridge = new CBridge(x, y, "ANI_WHITE_RED_STEEL_BRIDGE");
                scene->blocks.push_back(bridge);
                scene->foregrounds.push_back(bridge);
                break;
            }
            case 35:
            {
                CAxe *axe = new CAxe(x, y);
                scene->blocks.push_back((CBlock *)axe);
                break;
            }
            case 36:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_CHAIN"));
                break;
            case 37:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_WHITE_PILLAR"));
                break;
            case 38:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_WHITE_PILLAR"));
                break;

            case 50:
                scene->blocks.push_back(new CBrick(x, y, "ANI_GREEN_GROUND_BRICK"));
                break;
            case 51:
                scene->blocks.push_back(new CBrick(x, y, "ANI_GREEN_BREAKABLE_BRICK"));
                break;
            case 52:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_RED_CORAL"));
                break;
            case 53:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_WATER_WAVE_TOP"));
                break;
            case 54:
                scene->decors.push_back(new CDecorBlock(x, y, "ANI_WATER_WAVE_BODY"));
                break;

            case 138: // Used Lucky Block
            {
                scene->blocks.push_back(new CLuckyBlock(x, y, true)); // true = EMPTY
                break;
            }

            case 139: // Fire Circle (Used Lucky Block + 5 Fire Orbs)
            {
                scene->blocks.push_back(new CLuckyBlock(x, y, true)); // true = EMPTY
                
                // Random chiều quay: -1.0f (clockwise) hoặc 1.0f (counter_clockwise)
                float directionSign = (rand() % 2 == 0) ? -1.0f : 1.0f;
                // Random tốc độ trong khoảng 0.0015f đến 0.003f
                float baseSpeed = 0.0015f + ((float)(rand() % 151) / 100000.0f);
                float speed = directionSign * baseSpeed;

                for (int k = 0; k < 5; k++)
                {
                    CFireBar *orb = new CFireBar(x + 4, y + 4, speed);
                    orb->SetOffset((float)(k * 9)); // 9px apart for 8px orbs
                    scene->enemies.push_back(orb);
                }
                break;
            }

            default:
                DebugOut(L"[WARNING] Unknown tile ID %d at col=%d, row=%d\n", id, j, i);
                break;
            }
        }
    }

    DebugOut(L"[INFO] Parsed TILEMAP: %d blocks, %d decors created.\n",
             (int)scene->blocks.size(), (int)scene->decors.size());
}

void CMapLoader::_ParseObjectLine(const std::string &line, CPlayScene *scene)
{
    auto parts = _Split(line, ',');
    if (parts.size() < 3)
        return;

    std::string type = parts[0];

    // Skip header line
    if (type == "type")
        return;

    float x = std::stof(parts[1]);
    float y = std::stof(parts[2]);
    // The objects in level_1_1.csv are exported using a Y-down coordinate system.
    // Tiled exports the TOP-LEFT corner of the object. Since most of our entities (Mario, Goomba, Items) are 16x16,
    // we must subtract 16 to correctly find their BOTTOM-LEFT anchor in our Y-up world ecosystem.
    y = mapHeight - y - 16.0f;
    bool hidden_in_block = false;
    bool invisible_block = false;
    std::string dest_map = "";
    std::string enter_direction = "";
    float height = 0;
    float patrol_left = 0;
    float patrol_right = 0;
    std::string enemy_type = "green_walking";
    std::string name = "";
    std::string dest_pipe = "";

    for (size_t i = 3; i < parts.size(); ++i)
    {
        if (parts[i].find("name=") == 0)
            name = parts[i].substr(5);
        if (parts[i].find("dest_pipe=") == 0)
            dest_pipe = parts[i].substr(10);
        if (parts[i] == "hidden_in_block=true")
            hidden_in_block = true;
        if (parts[i] == "invisible_block=true")
            invisible_block = true;
        if (parts[i].find("dest_map=") == 0)
            dest_map = parts[i].substr(9);
        if (parts[i].find("enter_direction=") == 0)
            enter_direction = parts[i].substr(16);
        if (parts[i].find("height=") == 0)
            height = std::stof(parts[i].substr(7));
        if (parts[i].find("patrol_left=") == 0)
            patrol_left = std::stof(parts[i].substr(12));
        if (parts[i].find("patrol_right=") == 0)
            patrol_right = std::stof(parts[i].substr(13));
        if (parts[i].find("type=") == 0)
            enemy_type = parts[i].substr(5);
    }

    CGameObject *spawnedObj = nullptr;

    if (type == "mario")
    {
        if (scene->mario == nullptr)
            scene->mario = new CMario();
        if (CMario::hasCheckpoint)
        {
            scene->mario->x = CMario::checkpointX;
            scene->mario->y = CMario::checkpointY;
        }
        else
        {
            scene->mario->x = x;
            scene->mario->y = y;
        }
        spawnedObj = scene->mario;
        DebugOut(L"[INFO] Object: Mario at (%.2f, %.2f)\n", x, y);
    }
    else if (type == "goomba")
    {
        spawnedObj = new CGoomba(x, y, patrol_left, patrol_right);
        scene->enemies.push_back(spawnedObj);
    }
    else if (type == "koopa")
    {
        spawnedObj = new CKoopa(x, y, enemy_type, patrol_left, patrol_right);
        scene->enemies.push_back(spawnedObj);
    }
    else if (type == "mushroom" || type == "1-up")
    {
        spawnedObj = new CMushroom(x, y);
        scene->items.push_back(spawnedObj);
    }
    else if (type == "star")
    {
        spawnedObj = new CStar(x, y);
        scene->items.push_back(spawnedObj);
    }
    else if (type == "flower")
    {
        spawnedObj = new CFireFlower(x, y);
        scene->items.push_back(spawnedObj);
    }
    else if (type == "coin")
    {
        spawnedObj = new CCoin(x, y, hidden_in_block);
        scene->items.push_back(spawnedObj);
    }
    else if (type == "cheep_cheep")
    {
        float limit = 32.0f;
        for (auto p : parts)
            if (p.find("limit=") == 0)
                limit = std::stof(p.substr(6)) * 16.0f;
        auto cheep = new CCheepCheep(x, y);
        cheep->SetLimit(limit);
        spawnedObj = cheep;
        scene->enemies.push_back(spawnedObj);
    }
    else if (type == "firebar")
    {
        spawnedObj = new CFireBar(x, y);
        scene->enemies.push_back(spawnedObj);
    }
    else if (type == "bowser")
    {
        spawnedObj = new CBowser(x, y);
        scene->enemies.push_back(spawnedObj);
    }
    else if (type == "fire_circle")
    {
        std::string direction = "counter_clockwise";
        float baseSpeed = 0.002f;
        for (auto p : parts)
        {
            if (p.find("direction=") == 0)
                direction = p.substr(10);
            else if (p.find("speed=") == 0)
                baseSpeed = std::stof(p.substr(6));
        }

        // Tạo Lucky Block rỗng làm lõi quay
        scene->blocks.push_back(new CLuckyBlock(x, y, true)); // true = EMPTY

        float speed = (direction == "clockwise") ? -baseSpeed : baseSpeed;

        // Truyền thẳng tốc độ quay vào Constructor
        for (int k = 0; k < 5; k++)
        {
            CFireBar *orb = new CFireBar(x + 4, y + 4, speed);
            orb->SetOffset((float)(k * 9));
            scene->enemies.push_back(orb);
        }
    }
    else if (type == "bowser_fireball")
    {
        spawnedObj = new CBowserFireball(x, y);
        scene->enemies.push_back(spawnedObj);
    }
    else if (type == "podoboo")
    {
        spawnedObj = new CPodoboo(x, y);
        scene->enemies.push_back(spawnedObj);
    }
    else if (type == "blooper")
    {
        spawnedObj = new CBlooper(x, y);
        scene->enemies.push_back(spawnedObj);
    }
    else if (type == "piranha")
    {
        spawnedObj = new CPiranhaPlant(x, y);
        scene->enemies.push_back(spawnedObj);
    }
    else if (type == "platform" || type == "moving_platform")
    {
        float dir = 1.0f;
        float speed = 0.05f;
        std::string direction = "vertical";
        for (auto p : parts)
        {
            if (p.find("dir=") == 0)
                dir = std::stof(p.substr(4));
            if (p.find("direction=") == 0)
                direction = p.substr(10);
            if (p.find("speed=") == 0)
                speed = std::stof(p.substr(6));
        }
        CPlatform *plat = new CPlatform(x, y, dir);
        plat->SetSpeed(speed);
        plat->SetIsVertical(direction == "vertical");
        spawnedObj = plat;
        scene->blocks.push_back((CBlock *)spawnedObj);
    }
    else if (type == "npc_toad")
    {
        spawnedObj = new CDecorBlock(x, y, "ANI_NPC_TOAD");
        scene->decors.push_back((CDecorBlock *)spawnedObj);
    }
    else if (type == "lucky_block")
    {
        std::string item = "coin";
        for (auto p : parts)
            if (p.find("hidden_item=") == 0)
                item = p.substr(12);

        CLuckyBlock *lucky = new CLuckyBlock(x, y);
        if (item == "mushroom")
            lucky->SetHiddenItem(new CMushroom(x, y));
        else if (item == "fireflower")
            lucky->SetHiddenItem(new CFireFlower(x, y));
        else
            lucky->SetHiddenItem(new CCoin(x, y, true));

        scene->blocks.push_back((CBlock *)lucky);
    }
    else if (type == "castle_bridge")
    {
        scene->blocks.push_back((CBlock *)new CCastleBridge(x, y));
    }
    else if (type == "cloud")
    {
        std::string size = "small";
        for (auto p : parts)
            if (p.find("size=") == 0)
                size = p.substr(5);
        CDecorBlock *cloud;
        if (size == "large")
            cloud = new CDecorBlock(x, y, "ANI_CLOUD_BIG_ASSEMBLED");
        else
            cloud = new CDecorBlock(x, y, "ANI_CLOUD_SMALL_ASSEMBLED");
        cloud->SetDrifting(true);
        scene->decors.push_back(cloud);
    }
    else if (type == "pipe")
    {
        float topEdgeY = y + 16.0f;
        float pipeHeight = height;
        float pipeBottomY = y;

        if (pipeHeight <= 0)
        {
            // Backward compatibility: calculate height relative to groundY=32 if not specified
            float groundY = 32.0f;
            pipeHeight = topEdgeY - groundY;
            pipeBottomY = groundY;
        }

        if (pipeHeight <= 0)
            pipeHeight = 32.0f; // Minimum fallback height

        std::string pipeAni = "ANI_PIPE_UPWARDS_ASSEMBLED";
        if (enter_direction == "right")
        {
            pipeAni = "ANI_PIPE_2WAYS_ASSEMBLED";
            pipeHeight = 64.0f;
        }
        CPipe *pipeObj = new CPipe(x, pipeBottomY, 32.0f, pipeHeight, pipeAni, dest_map, enter_direction);
        pipeObj->SetName(name);
        pipeObj->SetDestPipe(dest_pipe);
        scene->blocks.push_back(pipeObj);
    }
    // Decorative background objects — no collision
    else if (type == "hill_small")
    {
        scene->decors.push_back(new CDecorBlock(x, y, "ANI_HILL_SMALL_ASSEMBLED"));
    }
    else if (type == "hill_big")
    {
        scene->decors.push_back(new CDecorBlock(x, y, "ANI_HILL_BIG_ASSEMBLED"));
    }
    else if (type == "tree_small")
    {
        scene->decors.push_back(new CDecorBlock(x, y, "ANI_TREE_SMALL_ASSEMBLED"));
    }
    else if (type == "tree_big")
    {
        scene->decors.push_back(new CDecorBlock(x, y, "ANI_TREE_BIG_ASSEMBLED"));
    }
    else if (type == "flagpole")
    {
        // Use the full assembled flagpole sprite (10+ blocks high)
        scene->decors.push_back(new CDecorBlock(x, y, "ANI_FLAG_FULL_ASSEMBLED"));

        // The flag itself (initially near the top, ~150px up from base)
        scene->decors.push_back(new CDecorBlock(x - 8.0f, y + 150.0f, "ANI_FLAG_OW"));
    }
    else if (type == "start_castle")
    {
        scene->decors.push_back(new CDecorBlock(x, y + 16.0f, "ANI_2FLOORS_CASTLE_ASSEMBLED"));
    }
    else if (type == "end_castle")
    {
        scene->decors.push_back(new CDecorBlock(x, y + 16.0f, "ANI_3FLOORS_CASTLE_ASSEMBLED"));
    }

    if (spawnedObj && hidden_in_block)
    {
        if (invisible_block)
        {
            // Tạo khối Lucky Block ẩn tại cùng vị trí với đồng xu/nấm
            CLuckyBlock *lucky = new CLuckyBlock(x, y);
            lucky->SetInvisible(true);
            lucky->SetHiddenItem(spawnedObj);
            scene->blocks.push_back((CBlock *)lucky);

            // Khóa trạng thái vật phẩm ẩn
            if (auto m = dynamic_cast<CMushroom *>(spawnedObj))
                m->SetState(MUSHROOM_STATE_HIDDEN);
            if (auto s = dynamic_cast<CStar *>(spawnedObj))
                s->SetState(STAR_STATE_HIDDEN);
            if (auto f = dynamic_cast<CFireFlower *>(spawnedObj))
                f->SetState(FLOWER_STATE_HIDDEN);
            if (auto c = dynamic_cast<CCoin *>(spawnedObj))
                c->SetState(COIN_STATE_HIDDEN);
        }
        else
        {
            // Móc item vào Lucky Block tại cùng vị trí x, y
            for (auto b : scene->blocks)
            {
                if (auto lucky = dynamic_cast<CLuckyBlock *>(b))
                {
                    // Do sai số float, dùng khoảng cách
                    if (abs(lucky->x - x) < 2.0f && abs(lucky->y - y) < 2.0f)
                    {
                        lucky->SetHiddenItem(spawnedObj);
                        // Force item state to hidden
                        if (auto m = dynamic_cast<CMushroom *>(spawnedObj))
                            m->SetState(MUSHROOM_STATE_HIDDEN);
                        if (auto s = dynamic_cast<CStar *>(spawnedObj))
                            s->SetState(STAR_STATE_HIDDEN);
                        if (auto f = dynamic_cast<CFireFlower *>(spawnedObj))
                            f->SetState(FLOWER_STATE_HIDDEN);
                        if (auto c = dynamic_cast<CCoin *>(spawnedObj))
                            c->SetState(COIN_STATE_HIDDEN);
                        break;
                    }
                }
            }
        }
    }
}

std::vector<std::string> CMapLoader::_Split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        // Clean up any stray whitespace
        token.erase(std::remove(token.begin(), token.end(), '\r'), token.end());
        token.erase(std::remove(token.begin(), token.end(), '\n'), token.end());
        tokens.push_back(token);
    }
    return tokens;
}
