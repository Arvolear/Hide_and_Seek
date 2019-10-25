#pragma once

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <ctime>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class PoissonDisk
{
    public:
        struct GridInfo
        {
            float x, y;

            GridInfo()
            {
                this->x = this->y = -1.0;
            }
            
            GridInfo(float x, float y)
            {
                this->x = x;
                this->y = y;
            }
            
            GridInfo(const GridInfo& GI)
            {
                this->x = GI.x;
                this->y = GI.y;
            }
            
            GridInfo& operator=(const GridInfo& GI)
            {
                this->x = GI.x;
                this->y = GI.y;

                return *this;
            }
        };

    private:
        int height, width;
        float radius;

        float sampleSize;

        int gridHeight, gridWidth;

        vector < vector < GridInfo > > grid;
        vector < GridInfo > active;

        float distance(const GridInfo& GI0, const GridInfo& GI1);

        void addSample(const GridInfo& GI);
        GridInfo generateAround(const GridInfo& GI);
        bool withinExtent(const GridInfo& GI);
        bool near(const GridInfo& GI);

        void clear();

    public:
        PoissonDisk();

        void setRadius(float radius);
        void setSize(int width, int height);
        void setSize(vec2 size);

        void generate();

        vector < GridInfo > getDisk() const; 

        ~PoissonDisk();
};
