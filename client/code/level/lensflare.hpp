#pragma once

//openGL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL/SOIL.h>

using namespace std;
using namespace glm;

class LensFlare
{
    private:
        GLuint baseTexture, outTexture;
        GLuint lensDirtTexture, flareBurstTexture, flareGradientTexture;

        Shader* flareShader;
        Shader* scaleShader;

        GaussianBlur < ColorBuffer >* gaussianBlur;
        ColorBuffer* upscaleBuffer;
        ColorBuffer* downscaleBuffer;
        ColorBuffer* flareBuffer;

        RenderQuad* quad;

    public:
        LensFlare();

        void setBaseTexture(GLuint baseTexture);
        void loadHelperTextures(string dirPath);
        
        void genBuffer(int width, int height, float scaleFactor);
        void genBuffer(vec2 size, float scaleFactor);

        GLuint getTexture() const;

        void renderFlares();
        void render(Shader* shader);

        ~LensFlare();
};
