#include "CPlayScene.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/utils/debug.h"
#include "../../registry/CMapLoader.h"
#include "../../registry/CResourceRegistry.h"
#include "../../../engine/physics/CCollision.h"
#include "../../../engine/core/Game.h"
#include "../../../engine/input/KeyboardManager.h"
#include "../../entities/blocks/CPipe.h"

void CPlayScene::Load (const std::string& mapPath)
{
  auto registry = CResourceRegistry::GetInstance();
  registry->LoadResourcesForPlayScene();

  currentMapPath = mapPath;
  pendingMapPath = "";

  mario = nullptr; // MapLoader sẽ khởi tạo Mario
  CMapLoader::GetInstance()->Load(mapPath, this);

  if (mario == nullptr) {
    DebugOut(L"[WARNING] Mario not found, starting manual initialization.\n");
    mario = new CMario();
    mario->x = 100.0f;
    mario->y = 150.0f;
  }

  DebugOut(L"[INFO] CPlayScene::Load completed. Map: %hs, Blocks: %d, Decors: %d\n",
           mapPath.c_str(), (int)blocks.size(), (int)decors.size());
}

void CPlayScene::Update (float dt)
{
  mario->Update(dt);
  for (auto b : blocks) b->Update(dt);
  for (auto e : enemies) e->Update(dt);
  for (auto i : items) i->Update(dt);

  std::vector<CGameObject*> coObjectsForMario;
  coObjectsForMario.reserve(blocks.size() + enemies.size() + items.size());

  // Lọc các block ở trong khoảng 256 pixels quanh Mario để tối ưu collision checks
  for (auto b : blocks)
    if (abs(b->x - mario->x) < 256.0f && abs(b->y - mario->y) < 256.0f)
      coObjectsForMario.push_back(b);

  // Tương tự với enemy và item
  for (auto e : enemies)
    if (abs(e->x - mario->x) < 256.0f && abs(e->y - mario->y) < 256.0f)
      coObjectsForMario.push_back(e);
  for (auto i : items)
    if (abs(i->x - mario->x) < 256.0f && abs(i->y - mario->y) < 256.0f)
      coObjectsForMario.push_back(i);

  CCollision::ResolveCollision(mario, dt, coObjectsForMario);

  // Xử lý collision cho enemy và item với block
  std::vector<CGameObject*> coObjectsForOthers;
  coObjectsForOthers.reserve(blocks.size()); //
  for (auto b : blocks) coObjectsForOthers.push_back(b);

  for (auto e : enemies) CCollision::ResolveCollision(e, dt, coObjectsForOthers);
  for (auto i : items) CCollision::ResolveCollision(i, dt, coObjectsForOthers);

  mario->UpdateState();

  // Logic đi xuống ống nước
  auto kb = KeyboardManager::GetInstance();
  if (kb->IsKeyPressed('S') || kb->IsKeyPressed(VK_DOWN))
    for (auto b : blocks)
      if (auto pipe = dynamic_cast<CPipe*>(b))
        if (pipe->IsWarpPipe() && pipe->GetEnterDirection() == "down")
        {
          float ml, mt, mr, mb;
          mario->GetBoundingBox(ml, mt, mr, mb);
          float pl, pt, pr, pb;
          pipe->GetBoundingBox(pl, pt, pr, pb);

          // Chân của Mario phải cách đỉnh ống nước trong khoảng 2 pixel
          if (mt >= pb - 2.0f && mt <= pb + 2.0f && ml < pr && mr > pl)
          {
            DebugOut(L"[INFO] Entering pipe to map: %hs\n", pipe->GetDestMap().c_str());
            std::string dest = "content/levels/" + pipe->GetDestMap() + ".csv";
            TransitionToMap(dest);
            break;
          }
        }

  // Logic đi vào ống nước (từ bên trái) => cần cập nhật hoặc gộp với logic trên

  CCamera::GetInstance()->Update(mario->x, mario->y, (DWORD)dt);

  // Chuyển map
  if (!pendingMapPath.empty())
  {
    std::string nextMap = pendingMapPath;
    Unload();
    Load(nextMap);
  }
}

void CPlayScene::Render() {
  ID3DX10Sprite *spriteHandler = CGame::GetInstance()->GetSpriteHandler();
  ID3D10Device *pD3DDevice = CGame::GetInstance()->GetDirect3DDevice();
  FLOAT NewBlendFactor[4] = {0, 0, 0, 0};

  // LAYER 1: Background decorations (hills, clouds, bushes, flag, castle)
  // Rendered first so they appear BEHIND everything else.
  for (auto d : decors)
    d->Render();

  // Flush the decor layer to GPU before rendering foreground objects.
  // D3DX10_SPRITE_SORT_TEXTURE reorders sprites by texture within a batch,
  // which can cause later-drawn sprites to appear behind earlier ones.
  // By flushing here, we guarantee decors are committed to the framebuffer
  // first.
  spriteHandler->Flush();

  // LAYER 2: Foreground — blocks, items, enemies, mario (all in same batch)
  for (auto b : blocks)
    b->Render();
  for (auto i : items)
    i->Render();
  for (auto e : enemies)
    e->Render();
  mario->Render();
}

void CPlayScene::Unload() {
  delete mario;
  mario = nullptr;

  for (auto b : blocks)
    delete b;
  blocks.clear();

  for (auto d : decors)
    delete d;
  decors.clear();

  for (auto e : enemies)
    delete e;
  enemies.clear();

  for (auto i : items)
    delete i;
  items.clear();
}