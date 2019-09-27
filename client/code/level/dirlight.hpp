#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class DirLight
{
    private:
        vec3 direction;
        vec3 color;

        mat4 shadowView, shadowProjection;

        Sphere* sphere;

        DepthColorBuffer* shadowBuffer;
        GaussianBlur < DepthColorBuffer >* gaussianBlur;

        ColorBuffer* scatterBuffer;
        RadialBlur* radialBlur;
        
    public:
        DirLight();
        DirLight(vec3 direction, vec3 color);
       
        void genShadowBuffer(int width, int height, float blurScale = 1.0);
        void genShadowBuffer(vec2 size, float blurScale = 1.0);
        void genScatterBuffer(int width, int height, float blurScale = 1.0);
        void genScatterBuffer(vec2 size, float blurScale = 1.0);

        void genSphere(vec3 center, double radius, int depth);

        void setDirection(vec3 direction);
        void setColor(vec3 color);
        void setSphereColor(vec3 color);

        void setExposure(float exposure);
        void setDecay(float decay);
        void setDensity(float density);
        void setWeight(float weight);

        void setShadowProjection(mat4 projection);

        vec3 getDirection() const;
        DepthColorBuffer* getShadowBuffer() const;
        ColorBuffer* getScatterBuffer() const;
       
        void blurShadow(float intensity, float radius = 1.0);
        void blurScatter(vec2 center);

        void renderShadow(Shader* shader, GLuint index);
        void renderSphere(Shader* shader);
        void renderLight(Shader* shader);
        void updateShadowView(vec3 playerPosition);

        Sphere* getSphere() const;

        GLuint getShadowTexture() const;
        GLuint getScatterTexture() const;

        mat4 getShadowView() const;
        mat4 getShadowProjection() const;

        ~DirLight();
};
