#pragma once

#include <map>
#include <vector>
#include <string>

#include <tinyxml2/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class LevelLoader
{
    private:
        World* physicsWorld;

        string levelName;

        Spawner* spawner;

        map < string, PhysicsObject* > physicsObjects;
        
        vector < Player* > players;

        /* helpers */
        void loadPhysicsObject(XMLElement* physicsObjectElem, PhysicsObject*& PO, string name);
        void loadWeapon(XMLElement* weaponElem, Weapon*& WE, string name);

        /* main */
        void loadPhysicsObjects();
        void loadWeapons();

        void loadSoldiers();

        void loadSpawner();

    public:
        LevelLoader(World* physicsWorld);

        void loadLevel(string name);
        void updateLevel();

        void getSpawner(Spawner*& spawner);

        void getPhysicsObjectsData(map < string, PhysicsObject* > &physicsObjects) const;
        void getPlayersData(vector < Player* > &players) const;

        ~LevelLoader();
};
