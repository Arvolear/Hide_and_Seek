#include "../global/globaluse.hpp"

#include "spawner.hpp"

Spawner::Spawner(int clients)
{
    prev.resize(clients, -1);
}

void Spawner::addPosition(btVector3 position)
{
    positions.push_back(position);
}

btTransform Spawner::getTransform(int client)
{
    if (positions.empty())
    {
        btTransform transform;
        transform.setIdentity();

        return transform;
    }
    
    vector < pair < int, btVector3 > > potentialPos;

    for (size_t i = 0; i < positions.size(); i++)
    {
        bool ok = true;

        for (size_t j = 0; j < prev.size(); j++)
        {
            if ((int)i == prev[j])
            {
                ok = false;
                break;
            }
        }

        if (ok)
        {
            potentialPos.push_back({i, positions[i]});
        }
    }
    
    if (potentialPos.empty())
    {
        btTransform transform;
        transform.setIdentity();

        return transform;
    }

    int index = round(global.getRandomNumber() * (potentialPos.size() - 1));
    
    btTransform transform;

    transform.setIdentity();
    transform.setOrigin(potentialPos[index].second);

    prev[client] = potentialPos[index].first;

    return transform;
}
        
void Spawner::clearPrev(int client)
{
    prev[client] = -1;
}

Spawner::~Spawner() {}
