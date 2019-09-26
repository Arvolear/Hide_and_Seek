#pragma once

#include <string>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;
        
class Sphere
{
    private:
        struct Vertex
        {
            vec3 position;
            vec3 color;
        };

        vector < Vertex > vertices;

        GLuint VAO;
        GLuint VBO;

        vec3 color;

        void setUpSphere();
        void constructOcta(vec3 center, double radius);
        void constructSphere(vector < Vertex > localVertices, int curDepth, int maxDepth);
        void roundComputedSphere(vec3 center, double radius);

    public:
        Sphere();

        void construct(vec3 center, double radius, int depth);

        void setColor(vec3 color);

        void render(Shader* shader) const;

        ~Sphere();
};
