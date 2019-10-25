#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class SSAO
{
    private:
        ColorBuffer* colorBuffer;
        GaussianBlur < ColorBuffer >* gaussBlur;

        vector < vec3 > kernel;
        vector < vec3 > noise;

        float radius;
        float bias;
        float power;

        GLuint texture_noise;

        vec2 renderSize;

        float lerp(float a, float b, float f);

    public:
        SSAO();

        void genBuffer(int width, int height);
        void genBuffer(vec2 size);
        void genSampleKernel(int size);
        void genNoise(int size);

        void setSoftness(int softness);
        void setRadius(float radius);
        void setBias(float bias);
        void setPower(float power);

        void blur(int intensity, float radius);
        void renderInfo(Shader* shader);
        void renderSsao(Shader* shader);

        ColorBuffer* getBuffer() const;
        GLuint getTexture() const;

        ~SSAO();
};
