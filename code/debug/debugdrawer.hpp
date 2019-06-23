#pragma once

//bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

//openGL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class DebugDrawer : public btIDebugDraw
{
    private:
        GLfloat Lpos[6];
        GLfloat Lcolor[6];

        GLuint VAO, VBO;

        int debugMode;
        
        void init();

        Shader* shader;
        mat4 view;
        mat4 projection;

    public:
        DebugDrawer();

        void setDebugMode(int debugMode) override;
        int getDebugMode() const override;

        void drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) override;
        void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;

        /* must declare */
        void reportErrorWarning(const char *warningString) override;
        void draw3dText(const btVector3 &location, const char* textString) override;

        void applyViewProjection(Shader* shader, mat4 view, mat4 projection);
        void updateViewProjection();

        ~DebugDrawer();
};
