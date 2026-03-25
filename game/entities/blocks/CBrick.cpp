#include "CBrick.h"
#include "../../../engine/Graphics/Animations.h"

#include "../../registry/CResourceRegistry.h"

void CBrick::Render() {
    int aniId = CResourceRegistry::GetInstance()->GetID("ANI_BRICK_IDLE");
    CAnimations::GetInstance()->Get(aniId)->Render(x, y);
}
