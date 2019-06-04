#pragma once

#include <iostream>

#include <map>
#include <vector>
#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Level
{
    private:
        Window* window;
        World* physicsWorld;

        LevelLoader* levelLoader;
        
        string levelName;

        ColorBuffer* levelColorBuffer;

        Shader* gameObjectShader;
        Shader* dirShadowShader;
        Shader* skyBoxShader;
        
        Shader* debugShader;

        /* all objects in the level */
        map < string, GameObject* > gameObjects;
        vector < DirLight* > dirLights;

        SkyBox* skyBox;

        Camera* camera;
        mat4 projection;

    public:
        Level(Window* window, World* physicsWorld);
        
        void loadLevel(string level);
        void render();

        /* TODO */
        void addGameObject(GameObject* gameObject);
        void getGameObject(string name);

        void removeGameObject(GameObject* gameObject);
        void removeGameObject(string name);
        /* TODO */

        GLuint getRenderTexture() const;
        Camera* getPlayer() const;

        ~Level();
};
