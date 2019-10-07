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
    
        SSAO* sSAO;
        Atmosphere* atmosphere;
        SkyBox* skyBox;

        mat4 projection;
        ViewFrustum* viewFrustum;
        
        vector < Player* > players;

        /* helpers */
        void loadProjection(XMLElement* projElem, mat4 &proj);
        void loadAnimation(XMLElement* animationElem, Animation*& anim);

        void loadGraphicsObject(XMLElement* graphicsObjectElem, GameObject*& GO);
        void loadPhysicsObject(XMLElement* physicsObjectElem, GameObject*& GO);
        void loadDebugObject(XMLElement* debugObjectElem, GameObject*& GO);

        void loadGameObject(XMLElement* gameObjectElem, GameObject*& GO);

        void loadRifle(XMLElement* rifleElem, Rifle*& rifle);

        /* main */
        void loadGameObjects();
        void loadRifles();
        void loadPistols();
        void loadDirLight();
        void loadSkyBox();
        void loadAtmosphere();
        void loadSsao();

        void loadProjection();

        void loadPlayers();
        void loadSoldiers();

    public:
        LevelLoader(Window* window, World* physicsWorld);

        void loadLevel(string name);

        void getGameObjectsData(map < string, GameObject* > &gameObjects) const;
        void getDirLightData(vector < DirLight* > &dirLights) const;
        void getSkyBoxData(SkyBox*& skyBox) const;
        void getAtmosphereData(Atmosphere*& atmosphere) const;
        void getSsaoData(SSAO*& sSAO) const;

        /*******/
        void getProjectionData(mat4 &projection) const;
        void getViewFrustumData(ViewFrustum*& viewFrustum);

        void getPlayersData(vector < Player* > &players) const;

        ~LevelLoader();
};
