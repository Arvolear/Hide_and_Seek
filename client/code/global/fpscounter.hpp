#pragma once

#include <iostream>

using namespace std;

class FPSCounter
{
    private:
        double updateTime;

        double actualPrevTime;
        double prevTime;
        unsigned int frames;

        double actualFrameTime;
        double framesTime;

        double fps;

    public:
        FPSCounter(double updateTime = 0.25);

        void reset(double time);
        void update(double time);

        double getActualFrameTime() const;
        double getFramesTime() const;
        double getFPS() const;

        ~FPSCounter();
};
