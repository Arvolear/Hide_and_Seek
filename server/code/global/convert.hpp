#pragma once

//native
#include <climits>
#include <string>

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

using namespace std;

inline static string path(string p)
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

inline static double toRads(double angle)
{
    return angle / 180.0 * 3.14159265;
};

inline static double toDegs(double rads)
{
    return rads * 180.0 / 3.14159265;
};
