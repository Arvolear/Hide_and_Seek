#pragma once

#include <string>

using namespace std;

class Animation
{
    private:
        int id; 

        string startFrame; 
        string endFrame; 
        string speed; 
        
        int priority; 
        bool loop; 
        
    public:
        Animation();
        Animation(int id, string frames, string speed = "0.4", int priority = 10, bool loop = false);
            
        void setId(int id);
        void setFrames(string frames);
        void setSpeed(string speed);
        void setPriority(int priority);
        void setLoop(bool loop);
        
        int getId() const;
        string getStart() const;
        string getEnd() const;
        string getFrames() const;
        string getSpeed() const;
        int getPriority() const;
        bool getLoop() const;

        ~Animation();
};
