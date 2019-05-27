#pragma once

#include <iostream>

#include <map>
#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Level
{
    private:
        Window* window;
        LevelLoader* levelLoader;
        
        string levelName;

        ColorBuffer* levelColorBuffer;
        DepthBuffer* levelDepthBuffer;

        Shader* objectShader;
        Shader* dirShadowShader;
        Shader* skyBoxShader;

        map < size_t, GameObject* > gameObjects;
        map < PhysicsObject*, GameObject* > physicsObjects;

        DirLight* dirLight;
        SkyBox* skyBox;

        Camera* camera;
        mat4 view;
        mat4* projection;
        mat4* shadowProjection;

    public:
        Level(Window* window);
        
        void loadLevel(string level);
        void render();
        GLuint getRenderTexture() const;

        Camera* getPlayer() const;

        ~Level();
};
