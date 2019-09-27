#pragma once

#include <stdexcept>

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class GBuffer : public FrameBuffer
{
    private:
        GLuint depthBufferID;

    public:
        GBuffer();
       
        void genBuffer(double width, double height, unsigned int layouts = 7) override;
        void genBuffer(vec2 size, unsigned int layouts = 7) override;

        void clear(vec3 color = vec3(0)) override;

        void render(Shader* shader);

        ~GBuffer();
};
