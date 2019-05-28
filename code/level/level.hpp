#pragma once

#include <iostream>

#include <map>
#include <vector>
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

        Shader* gameObjectShader;
        Shader* dirShadowShader;
        Shader* skyBoxShader;

        map < unsigned long int, GameObject* > gameObjects;
        vector < DirLight* > dirLights;

        SkyBox* skyBox;

        Camera* camera;
        mat4 projection;

    public:
        Level(Window* window);
        
        void loadLevel(string level);
        void render();

        /* TODO */
        void addGameObject(GameObject* gameObject);
        void getGameObject(PhysicsObject* physicsObject);

        void removeGameObject(GameObject* gameObject);
        void removeGameObject(PhysicsObject* physicsObject);
        /* TODO */

        GLuint getRenderTexture() const;
        Camera* getPlayer() const;

        ~Level();
};
