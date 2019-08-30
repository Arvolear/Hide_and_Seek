#pragma once

#include <iostream>

#include <map>
#include <vector>
#include <string>

using namespace std;

class Level
{
    private:
        World* physicsWorld;

        LevelLoader* levelLoader;
        
        string levelName;

        /* all objects in the level */
        map < string, PhysicsObject* > physicsObjects;
        
        vector < Player* > players;

    public:
        Level(World* physicsWorld);
        
        void loadLevel(string level);

        void addPhysicsObject(PhysicsObject* physicsObject);
        PhysicsObject* getPhysicsObject(string name) const;

        void removePhysicsObject(PhysicsObject* physicsObject);
        void removePhysicsObject(string name);

        void update();
        
        Player* getPlayer(int id) const;
        vector < Player* > getPlayers() const;
        string getLevelName() const;

        ~Level();
};
