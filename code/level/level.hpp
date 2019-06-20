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
        
        /* [0] player is active */
        vector < Player* > players;

        mat4 projection;
        ViewFrustum* viewFrustum;

        /* DEBUG */
        int drawDebug;

    public:
        Level(Window* window, World* physicsWorld);
        
        void loadLevel(string level);
        void render();

        void addGameObject(GameObject* gameObject);
        GameObject* getGameObject(string name) const;

        void removeGameObject(GameObject* gameObject);
        void removeGameObject(string name);

        GLuint getRenderTexture() const;
        Player* getPlayer() const;

        /* DEBUG */
        void toggleDebug();
        void swapPlayers();

        ~Level();
};
