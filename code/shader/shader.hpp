#pragma once

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <cstdlib>
#include <cstring>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Shader
{
    private:
        GLuint ID;

    public:
        
        Shader();

        GLuint loadShaders(string vertex_file_path, string fragment_file_path);

        void use() const;
        
        void setMat4(string key, mat4 value);
        void setMat3(string key, mat3 value);
        
        void setVec4(string key, vec4 value);
        void setVec3(string key, vec3 value);

        void setFloat(string key, float value);
        void setInt(string key, int value);

        GLuint getID() const;

        ~Shader();
};
