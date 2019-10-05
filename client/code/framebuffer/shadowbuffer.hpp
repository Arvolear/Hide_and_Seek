#pragma once

#include <stdexcept>

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class ShadowBuffer : public ColorBuffer
{
    public:
        ShadowBuffer();
       
        void genBuffer(int width, int height, vector < FrameBufferData > data) override;
        void genBuffer(vec2 size, vector < FrameBufferData > data) override;

        ~ShadowBuffer();
};
