#pragma once
#include "../../../engine/input/CInputManager.h"
#include "../../../engine/input/InputState.h"
#include "../CGameObject.h"
#include "MarioConfig.h"

class CMario : public CGameObject {
public:
    static int lives; 
    static float checkpointX, checkpointY;
    static bool hasCheckpoint;

    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
    void UpdateState();
    void SetOnGround(bool v) { onGround = v; }
    bool IsOnGround() const { return onGround; }

private:
    EMarioState state = EMarioState::IDLE;
    int nx = 1;
    bool onGround = false;
    void HandleInput(const InputState &input, float dt);
    void ApplyPhysics(float dt);

public:
    void GrowToBig();
    EMarioPower GetPower() const { return power; }
    void SetPower(EMarioPower p) { power = p; }
    void StompBounce();
    bool IsInvincible() const;
    void BecomeInvincible(float time) { starTimer = time; }
    void Hurt();
    void TryShootFireball();
    void Die();

    bool IsDeadState() const { return state == EMarioState::DIE; }
    float GetDieTimer() const { return dieTimer; }

private:
    EMarioPower power = EMarioPower::SMALL;
    float untouchableTimer = 0.0f;
    float starTimer = 0.0f;
    float dieTimer = 0.0f;
};