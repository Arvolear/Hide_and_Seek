#include "animation.hpp"

Animation::Animation()
{
    id = -1;

    startFrame = "0";
    endFrame = "0";
    speed = "0.4"; //default is 0.2 ~ 24 frames per second
    priority = 10; //default priority is 10
    loop = false;
}

Animation::Animation(int id, string frames, string speed, int priority, bool loop)
{
    this->id = id;

    string start;
    string end;

    if (frames != "default")
    {
        start = frames.substr(0, frames.find(' '));
        end = frames.substr(frames.find(' ') + 1, frames.length());
    }
    else
    {
        start = frames;
        end = frames;
    }

    startFrame = start; //starting frame
    endFrame = end; //ending frame

    this->speed = speed;

    this->priority = priority;

    this->loop = loop;
}


void Animation::setId(int id)
{
    this->id = id;
}

void Animation::setFrames(string frames)
{
    string start;
    string end;

    if (frames != "default")
    {
        start = frames.substr(0, frames.find(' '));
        end = frames.substr(frames.find(' ') + 1, frames.length());
    }
    else
    {
        start = frames;
        end = frames;
    }

    startFrame = start; //starting frame
    endFrame = end; //ending frame
}

void Animation::setSpeed(string speed)
{
    this->speed = speed;
}

void Animation::setPriority(int priority)
{
    this->priority = priority;
}

void Animation::setLoop(bool loop)
{
    this->loop = loop;
}

int Animation::getId()
{
    return id;
}

string Animation::getStart()
{
    return startFrame;
}

string Animation::getEnd()
{
    return endFrame;
}

string Animation::getFrames()
{
    return (startFrame + " " + endFrame);
}

string Animation::getSpeed()
{
    return speed;
}

int Animation::getPriority()
{
    return priority;
}

bool Animation::getLoop()
{
    return loop;
}

Animation::~Animation(){}
