#include "enemies/CBowser.h"
#include "enemies/CFireBar.h"
#include "enemies/CPodoboo.h"
#include "blocks/CPlatform.h"
#include "blocks/CCastleBridge.h"
#include "player/CMario.h"
#include "../registry/CResourceRegistry.h"

#include <cmath>

// --- Render Implementations ---
void CBowser::Render() {
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_BOWSER_WALK", x, y, nx);
}

void CFireBar::Render() {
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_FIRE_BAR_ORB", x, y);
}

void CPodoboo::Render() {
    if (state == JUMPING) {
        CResourceRegistry::GetInstance()->RenderAnimation("ANI_FIRE_LAVA_SHOOT", x, y);
    }
}

void CPlatform::Render() {
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_MOVING_PLATFORM", x, y);
}

void CCastleBridge::Render() {
    CResourceRegistry::GetInstance()->RenderAnimation("ANI_BRIDGE_BLOCK", x, y);
}

void CBowser::Update(float dt) {
    // Simple back and forth movement or stay idle
    // For now, keep it simple
}
void CBowser::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    left = x; bottom = y; right = x + 32; top = y + 32;
}

void CFireBar::Update(float dt) {
    angle += 0.002f * dt; 
    x = centerX + offset * cos(angle);
    y = centerY + offset * sin(angle);
}
void CFireBar::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    left = x; bottom = y; right = x + 8; top = y + 8;
}
void CFireBar::OnCollision(CGameObject* other) {
    if (auto mario = dynamic_cast<CMario*>(other)) {
        mario->StartDeath();
    }
}

void CPodoboo::Update(float dt) {
    if (state == WAITING) {
        vy += dt; // Reuse vy as timer
        if (vy > 1500) { // Wait 1.5s
            state = JUMPING;
            vy = 0.32f; 
        }
    } else {
        y += vy * dt;
        vy -= 0.0004f * dt; 
        if (y < startY) {
            y = startY;
            state = WAITING;
            vy = 0;
        }
    }
}
void CPodoboo::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    left = x; bottom = y; right = x + 12; top = y + 14;
}
void CPodoboo::OnCollision(CGameObject* other) {
    if (auto mario = dynamic_cast<CMario*>(other)) {
        mario->StartDeath();
    }
}

void CPlatform::Update(float dt) {
    if (isVertical) {
        vx = 0;
        vy = speed;
        y += vy * dt;
        // Wrap around NES height (240px)
        if (speed > 0 && y > 240) y = -16;
        else if (speed < 0 && y < -16) y = 240;
    } else {
        vy = 0;
        vx = speed;
        x += vx * dt;
        // Horizontal platforms in 2-4 usually bounce between some limits
        // For simplicity, let's just make it bounce within a 48px range
        static float startX = -1;
        if (startX == -1) startX = x;
        if (abs(x - startX) > 48) {
            speed = -speed;
            x = (x > startX) ? startX + 48 : startX - 48;
        }
    }
}
void CPlatform::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    left = x; bottom = y; right = x + 24; top = y + 8;
}

void CCastleBridge::Update(float dt) {}
void CCastleBridge::GetBoundingBox(float& left, float& bottom, float& right, float& top) {
    left = x; bottom = y; right = x + 16; top = y + 16;
}
