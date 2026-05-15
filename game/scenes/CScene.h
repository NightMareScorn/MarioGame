#pragma once

#include <d3dx10.h>

class CScene {
protected:
    D3DXCOLOR clearColor = D3DXCOLOR(92.0f/255, 148.0f/255, 252.0f/255, 1.0f); // Default NES Sky Blue
public:
    virtual void Load() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void Unload() = 0;

    D3DXCOLOR GetClearColor() { return clearColor; }
};
