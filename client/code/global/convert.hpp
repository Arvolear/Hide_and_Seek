#pragma once

//native
#include <climits>

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

//opengl
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace glm;
using namespace Assimp;
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

inline static mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 from)
{
    mat4 to;

    to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1; to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
    to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2; to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
    to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3; to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
    to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4; to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;

    return to;
}

inline static aiMatrix4x4 glmToAiMatrix4x4(mat4 from)
{
    return aiMatrix4x4(
            from[0][0], from[1][0], from[2][0], from[3][0],
            from[0][1], from[1][1], from[2][1], from[3][1],
            from[0][2], from[1][2], from[2][2], from[3][2],
            from[0][3], from[1][3], from[2][3], from[3][3]
            );
}

inline static mat4 btScalar2glmMat4(btScalar *from) 
{
    return mat4(
            from[0], from[1], from[2], from[3],
            from[4], from[5], from[6], from[7],
            from[8], from[9], from[10], from[11],
            from[12], from[13], from[14], from[15]);
}

inline static btScalar* glmMat42BtScalar(mat4 from) 
{
    btScalar* res = new btScalar[16];

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            res[i * 4 + j] = from[i][j];      
        }
    }

    return res;
}

inline static btVector3 toBtVector3(vec3 from)
{
    return btVector3(from.x, from.y, from.z);
}

inline static vec3 toVec3(btVector3 from)
{
    return vec3(from.x(), from.y(), from.z());
}

inline static btQuaternion toBtQuaternion(quat from)
{
    return btQuaternion(toBtVector3(axis(from)), angle(from));
}

inline static quat toQuat(btQuaternion from)
{
    return quat(from.getAngle(), toVec3(from.getAxis()));
}

inline static double toRads(double angle)
{
    return angle / 180.0 * 3.14159265;
};

inline static double toDegs(double rads)
{
    return rads * 180.0 / 3.14159265;
};
