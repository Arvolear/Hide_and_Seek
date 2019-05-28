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
        string levelName;

        map < unsigned long int, GameObject* > gameObjects;
        vector < DirLight* > dirLights;

        SkyBox* skyBox;

        /* TO BE CHANGED INTO THE PLAYER */
        Camera* camera;

        void loadObjects();
        void loadDirLight();
        void loadSkyBox();

        void loadPlayer();

    public:
        LevelLoader(Window* window);

        void loadLevel(string name);

        void getGameObjectsData(map < unsigned long int, GameObject* > &gameObjects) const;
        void getDirLightData(vector < DirLight* > &dirLights) const;
        void getSkyBoxData(SkyBox*& skyBox) const;
        

        /*******/
        void getPlayerData(Camera*& player) const;

        ~LevelLoader();
};
