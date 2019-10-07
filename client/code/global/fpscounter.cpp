#include "fpscounter.hpp"

FPSCounter::FPSCounter(double updateTime)
{
    this->updateTime = updateTime;

    actualPrevTime = prevTime = actualFrameTime = framesTime = fps = 0.0;
    frames = 0;
}

void FPSCounter::reset(double time)
{
    actualPrevTime = actualFrameTime = framesTime = fps = 0.0;

    prevTime = time;
    frames = 0;
}

void FPSCounter::update(double time)
{
    actualFrameTime = time - actualPrevTime;
    actualPrevTime = time;

    frames++;

    if (time - prevTime >= updateTime && frames > 0)
    {
        framesTime = updateTime / double(frames);
        fps = double(frames) / (time - prevTime);
        
        prevTime = time;
        frames = 0;
    }
}
        
double FPSCounter::getActualFrameTime() const
{
    return actualFrameTime;
}

double FPSCounter::getFramesTime() const
{
    return framesTime;
}

double FPSCounter::getFPS() const
{
    return fps;
}

FPSCounter::~FPSCounter() {}
