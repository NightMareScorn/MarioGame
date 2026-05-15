#include "CPipe.h"
#include "../../../engine/Graphics/Animations.h"

CPipe::CPipe(float x, float y, float width, float height, std::string aniName, std::string destMap, std::string enterDirection)
    : CBlock(x, y), width(width), height(height), aniName(aniName), destMap(destMap), enterDirection(enterDirection) {}

void CPipe::Render() {
    // Pipe is built from individual 32x16 tile pieces:
    //   - Head (opening): 1 row at the top (ANI_PIPE_OW_HEAD)
    //   - Body (shaft):   repeating rows below (ANI_PIPE_OW_BODY)
    //
    // We render from top to bottom. The pipe top is at (x, y + height - 16)
    // and extends downward to (x, y).

    float tileH = 16.0f;
    int totalTiles = (int)(height / tileH);
    if (totalTiles <= 0) return;

    // Top tile = head
    float topY = y + height - tileH;
    CAnimations::GetInstance()->Render("ANI_PIPE_OW_HEAD", x, topY);

    // Remaining tiles = body segments
    for (int t = 1; t < totalTiles; t++) {
        float segY = topY - t * tileH;
        CAnimations::GetInstance()->Render("ANI_PIPE_OW_BODY", x, segY);
    }
}

void CPipe::GetBoundingBox(float &l, float &t, float &r, float &b) {
    l = x;
    t = y;
    r = x + width;
    b = y + height;
}
