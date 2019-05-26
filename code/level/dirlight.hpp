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

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        
    public:
        DirLight();
        DirLight(vec3 dir, vec3 amb, vec3 diff, vec3 spec);
        
        void setDirection(vec3 dir);
        void setAmbient(vec3 amb);
        void setDiffuse(vec3 diff);
        void setSpecular(vec3 spec);

        vec3 getDirection() const;
        
        void render(Shader* shader);

        ~DirLight();
};
