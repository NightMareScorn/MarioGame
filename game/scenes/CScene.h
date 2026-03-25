#pragma once

class CScene {
public:
    virtual void Load() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void Unload() = 0;
};
