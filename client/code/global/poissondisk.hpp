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
    private:
        vec2 leftTop, rightBottom;
        int height, width;
        float radius;
        vector < vector < vec2 > > without; 

        vector < vec2 > initSamples;

        float sampleSize;

        int gridHeight, gridWidth;

        vector < vector < vec2 > > grid;
        vector < vec2 > active;

        void addSample(vec2 sample);
        vec2 generateAround(vec2 sample);
        bool withinExtent(vec2 sample);
        bool near(vec2 sample);

        bool onSegment(vec2 p0, vec2 q, vec2 p1);
        int orientation(vec2 p0, vec2 p1, vec2 p2);
        bool intersects(vec2 p0, vec2 q0, vec2 p1, vec2 q1);
        bool inside(vector < vec2 > polygon, vec2 p);

        void clear();

    public:
        PoissonDisk();

        void setRadius(float radius);
        void setBorders(vec2 leftTop, vec2 rightBottom);
        void setWithoutPolygons(vector < vector < vec2 > > &without);

        void generate();

        vector < vec2 > getDisk() const; 

        ~PoissonDisk();
};
