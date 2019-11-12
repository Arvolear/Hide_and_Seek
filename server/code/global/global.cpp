#include "global.hpp"

Global::Global() 
{
    gen = new mt19937(rd());
    dis = new uniform_real_distribution < float >(0.0, 1.0);
    
    start = chrono::system_clock::now();
}

string Global::path(string p) const
{
    char realPath[PATH_MAX];
    char* ptr;

    ptr = realpath(p.c_str(), realPath);

    /* error */
    if (ptr)
    {
        return ptr;
    }

    return {realPath};
}

float Global::cutFloat(float from, int precision) const
{
    stringstream ss;

    ss << fixed << setprecision(precision);

    ss << from;

    return stof(ss.str());
}

float Global::getRandomNumber() const
{
    return (*dis)(*gen);
}

float Global::getTime() const
{
    auto now = chrono::system_clock::now();

    chrono::duration < float > diff = now - start;
    
    return diff.count();
}

double Global::toRads(double angle) const
{
    return angle / 180.0 * 3.14159265;
}

double Global::toDegs(double rads) const
{
    return rads * 180.0 / 3.14159265;
}

Global::~Global() 
{
    delete gen;
    delete dis;
}
