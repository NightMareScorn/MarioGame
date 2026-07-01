#include "CCheepCheep.h"
#include <cmath>

void CCheepCheep::Update(float dt) {
    if (state == GOOMBA_STATE_DIE) return;

    float speed = GOOMBA_WALKING_SPEED * 0.63f;

    // If collision stopped us from moving horizontally, turn around immediately!
    if (vx == 0) {
        if (nx == 1) { // Hit left wall
            nx = -1;
            vx = speed;
        } else { // Hit right wall
            nx = 1;
            vx = -speed;
        }
    }

    // Cheep Cheeps move in a range without falling
    x += vx * dt;
    y += vy * dt; // vy should be 0 usually

    if (abs(x - start_x) > limit) {
        if (x > start_x) {
            x = start_x + limit;
            vx = -speed;
            nx = 1; // Face left (unflipped)
        } else {
            x = start_x - limit;
            vx = speed;
            nx = -1; // Face right (flipped)
        }
    }
}
