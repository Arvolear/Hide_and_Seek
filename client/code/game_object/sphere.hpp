#pragma once

#include <string>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

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
        vec3 center;
        double radius;

        mat4 localTransform;

        void setUpSphere();
        void constructOcta(vec3 center, double radius);
        void constructSphere(vector < Vertex > localVertices, int curDepth, int maxDepth);
        void roundComputedSphere(vec3 center, double radius);

    public:
        Sphere();

        void construct(vec3 center, double radius, int depth);
        void construct(int depth);

        void setCenter(vec3 center);

        void setColor(vec3 color);

        void render(Shader* shader) const;

        vec3 getCenter() const;
        double getRadius() const;

        ~Sphere();
};
