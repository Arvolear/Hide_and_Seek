#pragma once

//openGL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Bloom
{
    private:
        GLuint bloomTexture;
        GaussianBlur < ColorBuffer >* gaussianBlur;

    public:
        Bloom();
        
        void setBloomTexture(GLuint bloomTexture);

        void genBlurBuffer(int width, int height, float blurScale);
        void genBlurBuffer(vec2 size, float blurScale);

        void blurBloom(int intensity, float radius = 1.0);

        GLuint getTexture() const;

        void render(Shader* shader);

        ~Bloom();
};
