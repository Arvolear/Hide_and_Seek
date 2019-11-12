#pragma once

//native
#include <climits>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <random>

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

class Global
{
    private:
        random_device rd;
        mt19937* gen;
        uniform_real_distribution < float >* dis;
        
    public:
        static FPSCounter* fpsCounter;

        Global();

        string path(string p);
        
        float cutFloat(float from, int precision);

        float getRandomNumber();
        vec3 getRandomVec3();

        mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 from);
        aiMatrix4x4 glmToAiMatrix4x4(mat4 from);
        
        mat4 btScalar2glmMat4(btScalar *from);
        btScalar* glmMat42BtScalar(mat4 from);
        
        btVector3 toBtVector3(vec3 from);
        vec3 toVec3(btVector3 from);
        
        btQuaternion toBtQuaternion(quat from);
        quat toQuat(btQuaternion from);

        double toRads(double angle);
        double toDegs(double rads);

        ~Global();
};
