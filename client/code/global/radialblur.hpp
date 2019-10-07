#pragma once

//opengl
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class RadialBlur
{
    private:
        Shader* blurShader; 
        Shader* scaleShader; 

        ColorBuffer* colorBuffer;
        ColorBuffer* upscaleBuffer;
        ColorBuffer* downscaleBuffer;
        RenderQuad* quad;

        GLuint bluredTexture;

        float exposure, decay, density, weight;

    public:
        RadialBlur();
        RadialBlur(float exposure, float decay, float density, float weight);

        void genBuffer(int width, int height, FrameBufferData data, float scaleFactor = 1);
        void genBuffer(vec2 size, FrameBufferData data, float scaleFactor = 1);

        void setExposure(float exposure);
        void setDecay(float decay);
        void setDensity(float density);
        void setWeight(float weight);

        GLuint blur(GLuint textureID, vec2 center);

        GLuint getTexture() const;
        GLuint getBuffer() const;

        ~RadialBlur();
};
