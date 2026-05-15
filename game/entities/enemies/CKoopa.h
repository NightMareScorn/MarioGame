#pragma once
#include "../CGameObject.h"
#include <string>

class CKoopa : public CGameObject {
    std::string type;
public:
    CKoopa(float x, float y, std::string type = "green_walking", float patrolLeft = 0, float patrolRight = 0);
    void Update(float dt) override;
    void Render() override;
    void GetBoundingBox(float &l, float &t, float &r, float &b) override;
};
