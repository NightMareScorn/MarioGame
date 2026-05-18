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
    void PowerUpMushroom(); // Ăn nấm thành BIG
    void PowerUpFlower();
    void GrowToBig();
    EMarioPower GetPower() const { return power; }
    void SetPower(EMarioPower p) { power = p; }
    void StompBounce();
    bool IsInvincible() const;
    void BecomeInvincible(float time) { starTimer = time; }
    void Hurt();
    void TryShootFireball();
    void Die();
    void HitGoal(); // Hàm khi chạm cột cờ

    // THÊM 2 HÀM NÀY ĐỂ FIX LỖI "NO MEMBER"
    void SetInputLocked(bool v) { inputLocked = v; }
    bool IsInputLocked() const { return inputLocked; }

    bool IsDeadState() const { return state == EMarioState::DIE; }
    float GetDieTimer() const { return dieTimer; }
    EMarioState GetState() const { return state; }

private:
    EMarioState state = EMarioState::IDLE;
    bool onGround = false;
    bool inputLocked = false; // Biến quản lý khóa phím

    void HandleInput(const InputState &input, float dt);
    void ApplyPhysics(float dt);

    EMarioPower power = EMarioPower::SMALL;
    float untouchableTimer = 0.0f;
    float starTimer = 0.0f;
    float dieTimer = 0.0f;
};