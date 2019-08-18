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
        GBuffer* gBuffer;

        Shader* gBufferShader;
        Shader* gameObjectShader;
        Shader* dirShadowShader;
        Shader* skyBoxShader;
        
        Shader* debugShader;

        /* all objects in the level */
        map < string, GameObject* > gameObjects;
        vector < DirLight* > dirLights;

        SkyBox* skyBox;
        
        int playerID;
        vector < Player* > players;

        ViewFrustum* viewFrustum;
        RenderQuad* quad;
        
        mat4 projection;

        /* DEBUG */
        int drawDebug;

    public:
        Level(Window* window, World* physicsWorld);
        
        void loadLevel(string level);

        void setPlayerID(int playerID);
        
        void addGameObject(GameObject* gameObject);
        GameObject* getGameObject(string name) const;

        void removeGameObject(GameObject* gameObject);
        void removeGameObject(string name);
        
        void render();
        void updatePlayers(int mode);

        GLuint getRenderTexture(unsigned int num = 0) const;
        Player* getPlayer(int id = -1) const;
        string getLevelName() const;

        /* DEBUG */
        void toggleDebug();
        void swapPlayers();

        ~Level();
};
