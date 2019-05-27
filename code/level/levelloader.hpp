#pragma once

#include <map>
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

        map < size_t, GameObject* > gameObjects;
        map < PhysicsObject*, GameObject* > physicsObjects;

        DirLight* dirLight;
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

        void getGameObjectsData(map < size_t, GameObject* > &gameObjects) const;
        void getPhysicsObjectsData(map < PhysicsObject*, GameObject* > &physicsObjects) const;
        void getDirLightData(DirLight*& dirLight) const;
        void getSkyBoxData(SkyBox*& skyBox) const;
        
        /*******/
        void getPlayerData(Camera*& player) const;

        ~LevelLoader();
};
