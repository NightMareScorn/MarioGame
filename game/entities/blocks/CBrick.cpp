#include "CBrick.h"
#include "../../../engine/Graphics/Animations.h"

#include "../../registry/CResourceRegistry.h"

void CBrick::Render() {
    CAnimations::GetInstance()->Render("ANI_BRICK_IDLE", x, y);
}
