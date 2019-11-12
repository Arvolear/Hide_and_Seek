#pragma once

//native
#include <climits>
#include <cmath>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

using namespace std;

class Global
{
    private:
        random_device rd;
        mt19937* gen;
        uniform_real_distribution < float >* dis;
        
        chrono::system_clock::time_point start;

    public:
        Global();

        string path(string p) const;
        
        float cutFloat(float from, int precision) const;

        float getRandomNumber() const;
        float getTime() const;

        double toRads(double angle) const;
        double toDegs(double rads) const;

        ~Global();
};
