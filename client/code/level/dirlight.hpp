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
        float coeff;
        float shadowViewOffset;

        mat4 shadowView, shadowProjection;

        Sphere* sphere;

        DirLightSoftShadow* shadow;

        ColorBuffer* scatterBuffer;
        RadialBlur* radialBlur;
        
    public:
        DirLight();
        DirLight(vec3 direction, vec3 color);
       
        void genShadowBuffer(int width, int height);
        void genShadowBuffer(vec2 size);
        void genScatterBuffer(int width, int height, float blurScale = 1.0);
        void genScatterBuffer(vec2 size, float blurScale = 1.0);

        void genSphere(vec3 center, double radius, int depth);

        void setDirection(vec3 direction);
        void setColor(vec3 color);
        void setColorCoeff(float coeff);
        void setSphereColor(vec3 color);

        void setRadialExposure(float exposure);
        void setRadialDecay(float decay);
        void setRadialDensity(float density);
        void setRadialWeight(float weight);
      
        void setShadowIntensity(float intensity);
        void setShadowBias(float bias);
        void setShadowSoftness(float softness);
        void setShadowViewOffset(float offset);

        void setShadowProjection(mat4 projection);

        vec3 getDirection() const;
        ShadowBuffer* getShadowBuffer() const;
        ColorBuffer* getScatterBuffer() const;
       
        void blurShadow(int intensity, float radius);
        void blurScatter(vec2 center);

        void renderShadow(Shader* shader, GLuint index);
        void renderSphere(Shader* shader);
        void renderLight(Shader* shader);
        void updateShadowView(vec3 playerPosition, vec3 playerForward);

        Sphere* getSphere() const;

        GLuint getShadowTexture() const;
        GLuint getScatterTexture() const;

        mat4 getShadowView() const;
        mat4 getShadowProjection() const;

        ~DirLight();
};
