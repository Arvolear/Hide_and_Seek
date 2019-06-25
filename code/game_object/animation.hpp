#pragma once

#include <string>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Animation
{
    private:
        string name;
        int animId; 

        vec2 framesRange;
        float speed;
        bool loop; 
        
    public:
        Animation(string name);
        Animation(string name, int animId, vec2 framesRange = vec2(0), float speed = 0.0, bool loop = false);
        
        void setName(string name);
        void setAnimId(int animId);
        void setFramesRange(vec2 framesRange);
        void setSpeed(float speed);
        void setLoop(bool loop);
        
        string getName() const;
        int getAnimId() const;
        int getStartFrame() const;
        int getEndFrame() const;
        vec2 getFramesRange() const;
        float getSpeed() const;
        bool getLoop() const;

        ~Animation();
};
