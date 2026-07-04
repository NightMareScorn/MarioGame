#pragma once
#include "../../../engine/input/CInputManager.h"
#include "../../../engine/input/InputState.h"
#include "../CGameObject.h"
#include "MarioConfig.h"

class CMario : public CGameObject
{
public:
    static int lives;
    static float checkpointX, checkpointY;
    static bool hasCheckpoint;
    static EMarioPower currentPower;

    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
    void UpdateState();

    void SetOnGround(bool v) { onGround = v; }
    bool IsOnGround() const { return onGround; }
    void SetMapWidth(float w) { mapWidth = w; }

    void PowerUpMushroom(); // Ăn nấm thành BIG
    void PowerUpFlower();
    void GrowToBig();
    EMarioPower GetPower() const { return power; }
    void SetPower(EMarioPower p) { power = p; }
    void StompBounce();
    bool IsInvincible() const;
    void BecomeInvincible(float time);
    void Hurt();
    void TryShootFireball();
    void Die();
    void HitGoal(); // Hàm khi chạm cột cờ
    void SetState(int s);
    bool IsStarMode() const { return starTimer > 0; }
    bool IsBig() const { return power == EMarioPower::BIG || power == EMarioPower::BIG_FIRE; }
    void StartDeath() { Die(); }
    bool IsBlocking(CGameObject *other) override;

    void SetInputLocked(bool v) { inputLocked = v; }
    bool IsInputLocked() const { return inputLocked; }

    void ToggleGodMode() { isGodMode = !isGodMode; }
    bool IsGodMode() const { return isGodMode; }

    bool IsDeadState() const { return state == EMarioState::DIE; }
    float GetDieTimer() const { return dieTimer; }
    EMarioState GetState() const { return state; }
    bool IsDead() const { return isDead || state == EMarioState::DIE; }

    CMario();

private:
    EMarioState state = EMarioState::IDLE;
    bool onGround = false;
    bool isJumping = false;
    float jumpHoldTime = 0.0f;
    float mapWidth = 0.0f;
    bool isDead = false;

    bool inputLocked = false; // Biến quản lý khóa phím
    bool wasJumpKeyPressed = false;

    void HandleInput(const InputState &input, float dt);
    void ApplyPhysics(float dt);

    EMarioPower power = EMarioPower::SMALL;
    float untouchableTimer = 0.0f;
    float starTimer = 0.0f;
    float dieTimer = 0.0f;

    bool isPoweringUp = false;
    float powerUpTimer = 0.0f;
    bool isGodMode = false;
};