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

        void init();
        void checkEvents(); 

        double getTime();

        chrono::system_clock::time_point start;

    public:
        Game(string level);
        
        void gameLoop();

        ~Game();
};
