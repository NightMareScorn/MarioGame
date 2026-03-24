#pragma once
#include <Windows.h>

class Time
{
private:
    LARGE_INTEGER timerFreq;
    LARGE_INTEGER timeNow;
    float accumulator;
    float dt;

    int updateCount;
    int renderCount;
    float timeElapsedSec;

public:
    Time(float targetFPS = 60.0f);

    void Tick();
    bool ShouldUpdate();
    float GetDeltaTime() const;

    void OnUpdate();
    void OnRender();
    void PrintFPS();
};
