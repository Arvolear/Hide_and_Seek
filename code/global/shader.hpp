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

using namespace std;

class Shader
{
    private:
        GLuint ID;

    public:
        
        Shader();

        GLuint loadShaders(string vertex_file_path, string fragment_file_path);

        void use() const;
        GLuint getID() const;

        ~Shader();
};
