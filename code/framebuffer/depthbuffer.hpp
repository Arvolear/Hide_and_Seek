#pragma once

#include <stdexcept>

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class DepthBuffer : public FrameBuffer
{
    public:
        DepthBuffer();
       
        void genBuffer(double width, double height) override;
        void genBuffer(vec2 size) override;

        void clear() override;

        ~DepthBuffer();
};
