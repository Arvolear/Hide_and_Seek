#pragma once

#include <map>
#include <vector>
#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class LevelLoader
{
    private:
        Window* window;
        World* physicsWorld;

        string levelName;

        map < string, GameObject* > gameObjects;
        vector < DirLight* > dirLights;

        SkyBox* skyBox;

        /* TO BE CHANGED INTO THE PLAYER */
        Camera* camera;

        void loadObjects();
        void loadDirLight();
        void loadSkyBox();

        void loadPlayer();

    public:
        LevelLoader(Window* window, World* physicsWorld);

        void loadLevel(string name);

        void getGameObjectsData(map < string, GameObject* > &gameObjects) const;
        void getDirLightData(vector < DirLight* > &dirLights) const;
        void getSkyBoxData(SkyBox*& skyBox) const;
        

        /*******/
        void getPlayerData(Camera*& player) const;

        ~LevelLoader();
};
