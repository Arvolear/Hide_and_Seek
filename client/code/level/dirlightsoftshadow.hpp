#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class DirLightSoftShadow
{
    private:
        ShadowBuffer* shadowBuffer;
        GaussianBlur < ShadowBuffer >* gaussBlur;
        
        int width, height;

        float intensity;
        float bias;
        float softness;

    public:
        DirLightSoftShadow();

        void genBuffer(int width, int height);
        void genBuffer(vec2 size);

        void setIntensity(float intensity);
        void setBias(float bias);
        void setSoftness(float softness);

        void blurShadow(int intensity, float radius);

        void render(Shader* shader, int index);

        ShadowBuffer* getBuffer() const;
        GLuint getTexture() const;

        ~DirLightSoftShadow();
};
