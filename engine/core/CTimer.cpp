#include "CTimer.h"
#include <stdio.h>

CTimer::CTimer(float targetFPS)
{
    QueryPerformanceFrequency(&timerFreq);
    QueryPerformanceCounter(&timeNow);
    dt = 1.0f / targetFPS;
    accumulator = 0.0f;
    updateCount = 0;
    renderCount = 0;
    timeElapsedSec = 0.0f;
}

void CTimer::Tick()
{
    LARGE_INTEGER timeNext;
    QueryPerformanceCounter(&timeNext);
    float frameTime = (float)(timeNext.QuadPart - timeNow.QuadPart) / timerFreq.QuadPart;
    timeNow = timeNext;

    // Clamp frameTime to max 0.25s
    if (frameTime > 0.25f)
        frameTime = 0.25f;

    accumulator += frameTime;
    timeElapsedSec += frameTime;
}

bool CTimer::ShouldUpdate()
{
    if (accumulator >= dt)
    {
        accumulator -= dt;
        return true;
    }
    return false;
}

float CTimer::GetDeltaTime() const
{
    return dt;
}

void CTimer::OnUpdate()
{
    updateCount++;
}

void CTimer::OnRender()
{
    renderCount++;
}

void CTimer::PrintFPS()
{
    if (timeElapsedSec >= 1.0f)
    {
        printf("MarioGame | Update: %d | Render: %d\n", updateCount, renderCount);
        updateCount = 0;
        renderCount = 0;
        timeElapsedSec -= 1.0f;
    }
}
