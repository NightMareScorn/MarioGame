#include "CCheepCheep.h"
#include <cmath>

void CCheepCheep::Update(float dt) {
    if (state == GOOMBA_STATE_DIE) return;

    // Cheep Cheeps move in a range without falling
    x += vx * dt;
    y += vy * dt; // vy should be 0 usually

    if (abs(x - start_x) > limit) {
        if (x > start_x) {
            x = start_x + limit;
            vx = -abs(vx);
            nx = -1;
        } else {
            x = start_x - limit;
            vx = abs(vx);
            nx = 1;
        }
    }
}
