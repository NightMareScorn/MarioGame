#include "CPlayScene.h"
#include <algorithm>
#include "../../../engine/core/Game.h"
#include "../../../engine/input/KeyboardManager.h"
#include "../../../engine/physics/CCollision.h"
#include "../../../engine/rendering/Camera.h"
#include "../../../engine/utils/debug.h"
#include "../../entities/blocks/CPipe.h"
#include "../../registry/CMapLoader.h"
#include "../../registry/CResourceRegistry.h"

void CPlayScene::Load(const std::string &mapPath) {
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

  DebugOut(
      L"[INFO] CPlayScene::Load completed. Map: %hs, Blocks: %d, Decors: %d\n",
      mapPath.c_str(), (int)blocks.size(), (int)decors.size());
}

void CPlayScene::Update(float dt) {
  mario->Update(dt);
  for (auto b : blocks)
    b->Update(dt);
  for (auto e : enemies)
    e->Update(dt);
  for (auto i : items)
    i->Update(dt);

  std::vector<CGameObject *> coObjectsForMario;

  // Lọc các blocks, enemies và items ở trong khoảng 256 pixels quanh Mario để
  // tối ưu collision checks
  auto nearbyBlocks = GetObjectsInRange(mario->x, mario->y, blocks);
  auto nearbyEnemies = GetObjectsInRange(mario->x, mario->y, enemies);
  auto nearbyItems = GetObjectsInRange(mario->x, mario->y, items);

  coObjectsForMario.reserve(nearbyBlocks.size() + nearbyEnemies.size() +
                            nearbyItems.size());

  coObjectsForMario.insert(coObjectsForMario.end(), nearbyBlocks.begin(),
                           nearbyBlocks.end());
  coObjectsForMario.insert(coObjectsForMario.end(), nearbyEnemies.begin(),
                           nearbyEnemies.end());
  coObjectsForMario.insert(coObjectsForMario.end(), nearbyItems.begin(),
                           nearbyItems.end());

  CCollision::ResolveCollision(mario, dt, coObjectsForMario);

  // Xử lý collision cho từng enemy với block xung quanh nó
  for (auto e : enemies) {
    float old_vx = e->vx;

    auto blocksAroundEnemy = GetObjectsInRange(e->x, e->y, blocks);
    auto enemiesAroundEnemy = GetObjectsInRange(e->x, e->y, enemies);

    auto it = std::find(enemiesAroundEnemy.begin(), enemiesAroundEnemy.end(), e);
    if (it != enemiesAroundEnemy.end()) enemiesAroundEnemy.erase(it);

    std::vector<CGameObject*> coObjectsForEnemy = blocksAroundEnemy;
    coObjectsForEnemy.insert(coObjectsForEnemy.end(), enemiesAroundEnemy.begin(), enemiesAroundEnemy.end());

    CCollision::ResolveCollision(e, dt, coObjectsForEnemy);

    if (e->vx == 0 && old_vx != 0) {
      e->vx = -old_vx;
      e->nx = (e->vx > 0) ? 1 : -1;
    }

    if (e->vy == 0 && e->vx != 0) {
      float L, B, R, T;
      e->GetBoundingBox(L, B, R, T);

      CCollision::Box sensor;
      if (e->vx < 0) {
        sensor = CCollision::ToBox(L, B - 4.0f, L + 2.0f, B - 0.5f);
      } else {
        sensor = CCollision::ToBox(R - 2.0f, B - 4.0f, R, B - 0.5f);
      }

      bool hasFloor = false;
      for (auto b : blocksAroundEnemy) {
        float bl, bb, br, bt;
        b->GetBoundingBox(bl, bb, br, bt);
        if (CCollision::CheckAABB(sensor, CCollision::ToBox(bl, bb, br, bt))) {
          hasFloor = true;
          break;
        }
      }

      if (!hasFloor) {
        e->vx = -e->vx;
        e->nx = (e->vx > 0) ? 1 : -1;
      }
    }
  }

  // Xử lý collision cho từng item với block xung quanh nó
  for (auto i : items) {
    auto blocksAroundItem = GetObjectsInRange(i->x, i->y, blocks);
    CCollision::ResolveCollision(i, dt, blocksAroundItem);
  }

  mario->UpdateState();

  // Logic đi xuống ống nước
  auto kb = KeyboardManager::GetInstance();
  if (kb->IsKeyPressed('S') || kb->IsKeyPressed(VK_DOWN))
    for (auto b : blocks)
      if (auto pipe = dynamic_cast<CPipe *>(b))
        if (pipe->IsWarpPipe() && pipe->GetEnterDirection() == "down") {
          float mLeft, mBottom, mRight, mTop;
          mario->GetBoundingBox(mLeft, mBottom, mRight, mTop);
          float pLeft, pBottom, pRight, pTop;
          pipe->GetBoundingBox(pLeft, pBottom, pRight, pTop);

          // Chân của Mario phải cách đỉnh ống nước trong khoảng 2 pixel
          if (mBottom >= pTop - 2.0f && mBottom <= pTop + 2.0f &&
              mLeft < pRight && mRight > pLeft) {
            DebugOut(L"[INFO] Entering pipe to map: %hs\n",
                     pipe->GetDestMap().c_str());
            std::string dest = "content/levels/" + pipe->GetDestMap() + ".csv";
            TransitionToMap(dest);
            break;
          }
        }

  // TODO: Logic đi vào ống nước (từ bên trái) => Cần cập nhật hoặc gộp với
  // logic trên

  CCamera::GetInstance()->Update(mario->x, mario->y, (DWORD)dt);

  // Chuyển map
  if (!pendingMapPath.empty()) {
    std::string nextMap = pendingMapPath;
    Unload();
    Load(nextMap);
  }
}

void CPlayScene::Render() {
  ID3DX10Sprite *spriteHandler = CGame::GetInstance()->GetSpriteHandler();

  // Layer 1: Background (hills, clouds, bushes, flag, castle)
  for (auto d : decors)
    d->Render();
  // Đẩy lên GPU batch chứa riêng decors
  spriteHandler->Flush();

  // Layer 2: Foreground (blocks, items, enemies, mario)
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