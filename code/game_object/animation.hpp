#pragma once

#include <string>

using namespace std;

class Animation
{
    private:
        int id; //name of the animation

        string startFrame; //start time of the animation
        string endFrame; //end time of the animation
        string speed; //speed of the animation
        
        int priority; //priority of the animation
        bool loop; //is this animation looped
        
    public:
        Animation();
        Animation(int id, string frames, string speed = "0.4", int priority = 10, bool loop = false);
            
        void setId(int id);
        void setFrames(string frames);
        void setSpeed(string speed);
        void setPriority(int priority);
        void setLoop(bool loop);
        
        int getId();
        string getStart();
        string getEnd();
        string getFrames();
        string getSpeed();
        int getPriority();
        bool getLoop();

        ~Animation();
};
