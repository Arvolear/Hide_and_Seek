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
        string levelPath;

        ColorBuffer* levelColorBuffer;
        GBuffer* gBuffer;

        Shader* gBufferShader;
        Shader* gameObjectShader;
        Shader* dirShadowShader;
        Shader* skyBoxShader;
        Shader* dirSphereShader;
        Shader* lightBlenderShader;
        Shader* atmosphereShader;
        Shader* domeShader;
        Shader* sSAOShader;
        
        Shader* debugShader;

        /* all objects in the level */
        map < string, GameObject* > gameObjects;
        vector < DirLight* > dirLights;

        SSAO* sSAO; 
        Atmosphere* atmosphere;
        SkyBox* skyBox;
        
        int playerID;
        vector < Player* > players;      

        ViewFrustum* viewFrustum;
        RenderQuad* quad;
        
        mat4 projection;

        /* DEBUG */
        int drawDebug;
        Player* virtualPlayer;
        int activeVirtualPlayer;

    public:
        Level(Window* window, World* physicsWorld);
        
        void loadLevel(string level);

        void setPlayerID(int playerID);

        void addGameObject(GameObject* gameObject);
        GameObject* getGameObject(string name) const;
        map < string, GameObject* > getGameObjects() const;

        void removeGameObject(GameObject* gameObject);
        void removeGameObject(string name);
        
        void render();
        void updatePlayers(int mode);
        void updateSunPos();

        GLuint getRenderTexture(unsigned int num = 0) const;
        Player* getConnectedPlayer(bool andVirtual = false) const;
        Player* getIDPlayer(int id) const;
        vector < Player* > getPlayers() const;
        vec3 getSunPosition() const;

        string getLevelName() const;
        string getLevelPath() const;

        /* DEBUG */
        void toggleDebug();
        void toggleVirtualPlayer();

        ~Level();
};
