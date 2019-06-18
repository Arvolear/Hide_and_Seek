#pragma once

#include <map>
#include <vector>
#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace glm;
using namespace tinyxml2;

class LevelLoader
{
    private:
        Window* window;
        World* physicsWorld;

        string levelName;

        map < string, GameObject* > gameObjects;
        vector < DirLight* > dirLights;

        SkyBox* skyBox;

        mat4 projection;
        
        Player* player;

        void loadObjects();
        void loadDirLight();
        void loadSkyBox();

        void loadProjection();

        void loadPlayer();

    public:
        LevelLoader(Window* window, World* physicsWorld);

        void loadLevel(string name);

        void getGameObjectsData(map < string, GameObject* > &gameObjects) const;
        void getDirLightData(vector < DirLight* > &dirLights) const;
        void getSkyBoxData(SkyBox*& skyBox) const;
        

        /*******/
        void getProjectionData(mat4 &projection) const;
        void getPlayerData(Player*& player) const;

        ~LevelLoader();
};
