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
       
        void genBuffer(int width, int height, vector < FrameBufferData > data) override;
        void genBuffer(vec2 size, vector < FrameBufferData > data) override;

        void clear() override;
        void clearColor(vec4 color = vec4(0.0, 0.0, 0.0, 1.0)) override;
        void clearDepth(float depth  = 1.0) override;

        void render(Shader* shader);
        void renderSsao(Shader* shader);
        void renderStaticDepth(Shader* shader);

        ~GBuffer();
};
