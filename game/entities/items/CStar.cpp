#include "CStar.h"
#include "../../../engine/Graphics/Animations.h"
#include "../../entities/player/CMario.h"

CStar::CStar(float x, float y) : CGameObject() {
  this->x = x;
  this->y = y;
  this->vx = STAR_SPEED;
  SetState(STAR_STATE_HIDDEN);
}

void CStar::Update(float dt) {
  if (state == STAR_STATE_HIDDEN)
    return;

  vy -= STAR_GRAVITY * dt;

  // Bounce when hitting ground (vy gets zeroed by collision resolver)
  if (vy == 0 && state == STAR_STATE_MOVING) {
    vy = STAR_BOUNCE_FORCE; // Re-bounce upward
  }

  if (vx == 0) {
    vx = (nx > 0) ? STAR_SPEED : -STAR_SPEED;
    nx = -nx;
  }
}

void CStar::Render() {
  if (state == STAR_STATE_HIDDEN)
    return;
  CAnimations::GetInstance()->Render("ANI_STAR_OW", x, y);
}

void CStar::GetBoundingBox(float &left, float &bottom, float &right, float &top) {
  if (state == STAR_STATE_HIDDEN) {
    left = bottom = right = top = 0;
    return;
  }
  
  left = x;
  bottom = y;
  right = x + STAR_BBOX_WIDTH;
  top = y + STAR_BBOX_HEIGHT;
}

void CStar::SetState(int s) { state = s; }

void CStar::OnCollected(CMario* mario) {
    if (state != STAR_STATE_HIDDEN) {
        mario->BecomeInvincible(10.0f);
        this->SetIsDead(true);
    }
}
