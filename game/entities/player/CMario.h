#pragma once
#include "../../../engine/input/CInputManager.h"
#include "../../../engine/input/InputState.h"
#include "../CGameObject.h"
#include "MarioConfig.h"

class CMario : public CGameObject {
public:
  void Update(float dt) override;
  void Render() override;
  void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
  void UpdateState();
  void SetOnGround(bool v) { onGround = v; }
  bool IsOnGround() const { return onGround; }
  bool IsBig() const { return form == EMarioForm::BIG; }
  void SetForm(EMarioForm f) { form = f; }

private:
  EMarioState state = EMarioState::IDLE;
  EMarioForm form = EMarioForm::SMALL;
  int nx = 1; // 1 for right, -1 for left
  bool onGround = false;

  void HandleInput(const InputState &input, float dt);
  void ApplyPhysics(float dt);
};