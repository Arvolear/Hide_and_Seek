#pragma once

#include <chrono>
#include <thread>

using namespace std;

class Game
{
    private:
        Level* level;
        World* physicsWorld;

        Multiplayer* multiplayer;

        void checkEvents(); 

    public:
        Game(string level);
        
        void gameLoop();

        ~Game();
};
