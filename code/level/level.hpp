#pragma once

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
        //LevelLoader* levelLoader;

        ColorBuffer* levelColorBuffer;
        DepthBuffer* levelDepthBuffer;

        Shader* objectShader;
        Shader* dirShadowShader;

        map < size_t, GameObject* > gameObjects;
        map < PhysicsObject*, GameObject* > physicsObjects;

        DirLight* dirLight;
        SkyBox* skyBox;
        
        string levelName;

    public:
        Level(Window* window);
        
        void loadLevel(string level);
        void render();
        GLuint getRenderTexture() const;

        ~Level();
};
