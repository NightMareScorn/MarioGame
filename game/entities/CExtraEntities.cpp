#include "items/CFireFlower.h"
#include "enemies/CBowser.h"
#include "enemies/CFireBar.h"
#include "enemies/CPodoboo.h"
#include "blocks/CPlatform.h"
#include "blocks/CCastleBridge.h"
#include "../registry/CResourceRegistry.h"

// --- Render Implementations ---
void CFireFlower::Render() {
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_FLOWER_OW", x, y);
}

void CBowser::Render() {
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_BOWSER_IDLE", x, y, nx);
}

void CFireBar::Render() {
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_FIRE_BAR", x, y);
}

void CPodoboo::Render() {
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_FIRE_BAR", x, y);
}

void CPlatform::Render() {
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_MUSHROOM_PLATFORM_MID", x, y);
}

void CCastleBridge::Render() {
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_BRIDGE_BLOCK", x, y);
}

// --- Update & BBox Implementations (Stubs for now) ---
void CFireFlower::Update(float dt) {}
void CFireFlower::GetBoundingBox(float& l, float& t, float& r, float& b) {
    l = x; t = y; r = x + 16; b = y + 16;
}

void CBowser::Update(float dt) {}
void CBowser::GetBoundingBox(float& l, float& t, float& r, float& b) {
    l = x; t = y; r = x + 32; b = y + 32;
}

void CFireBar::Update(float dt) {}
void CFireBar::GetBoundingBox(float& l, float& t, float& r, float& b) {
    l = x; t = y; r = x + 8; b = y + 8;
}

void CPodoboo::Update(float dt) {}
void CPodoboo::GetBoundingBox(float& l, float& t, float& r, float& b) {
    l = x; t = y; r = x + 8; b = y + 8;
}

void CPlatform::Update(float dt) {}
void CPlatform::GetBoundingBox(float& l, float& t, float& r, float& b) {
    l = x; t = y; r = x + 48; b = y + 8;
}

void CCastleBridge::Update(float dt) {}
void CCastleBridge::GetBoundingBox(float& l, float& t, float& r, float& b) {
    l = x; t = y; r = x + 16; b = y + 16;
}
