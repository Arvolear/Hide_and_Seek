#pragma once

#include <string>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;
        
struct DebugVertex
{
    vec3 position;
    vec3 color;
};

class DebugSphere
{
    private:
        vector < DebugVertex > vertices;

        GLuint VAO;
        GLuint VBO;

        mat4 transform;

        void setUpSphere();
        void constructOcta(vec3 center, double radius);
        void constructSphere(vector < DebugVertex > localVertices, int curDepth, int maxDepth);
        void roundComputedSphere(vec3 center, double radius);

    public:
        DebugSphere();

        void construct(vec3 center, double radius, int depth);

        void applyTransform(mat4 &transform);

        void draw(Shader* shader) const;

        ~DebugSphere();
};
