#pragma once
#include "../CGameObject.h"
#include <string>

#define KOOPA_STATE_WALKING 100
#define KOOPA_STATE_SHELL 200
#define KOOPA_STATE_SHELL_RUNNING 300

class CKoopa : public CGameObject {
    std::string type;
    int state;
public:
    CKoopa(float x, float y, std::string type = "green_walking", float patrolLeft = 0, float patrolRight = 0);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &left, float &bottom, float &right, float &top) override;
    
    void SetState(int s) override;
    void OnCollisionX(CGameObject* other, float nx) override;
    void OnCollisionY(CGameObject* other, float ny) override;
    bool IsBlocking(CGameObject* other) override;
    int GetState() { return state; }

    bool IsEnemy() const override { return state == KOOPA_STATE_WALKING || state == KOOPA_STATE_SHELL_RUNNING; }
    void OnStomped() override { SetState(KOOPA_STATE_SHELL); }
};
