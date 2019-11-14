#include "../global/globaluse.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "../player/player.hpp"

#include "playerdisconnectioncollector.hpp"
        
PlayerDisconnectionCollector::PlayerDisconnectionCollector(int clients) 
{
    last.resize(clients, "");
}

void PlayerDisconnectionCollector::collect(vector < int > old_clients)
{
    this->old_clients = old_clients;
}

string PlayerDisconnectionCollector::getData(int client) const
{
    XMLDocument playerDisconnectionCollectorDoc;

    /* root */
    XMLNode* root = playerDisconnectionCollectorDoc.NewElement("Dis");
    
    int j = 0;
    for (size_t i = 0; i < old_clients.size(); i++)
    {
        if (old_clients[i] > 0)
        {
            string str;
            str = char('a' + j);

            dynamic_cast < XMLElement* >(root)->SetAttribute(str.data(), (int)i);
            j++;
        }
    }

    if (!j)
    {
        last[client] = "";
        return "";
    }

    playerDisconnectionCollectorDoc.InsertFirstChild(root);

    /* printer */
    XMLPrinter playerDisconnectionCollectorPrinter;
    playerDisconnectionCollectorDoc.Print(&playerDisconnectionCollectorPrinter);
    
    string res = playerDisconnectionCollectorPrinter.CStr();
    
    if (res == last[client])
    {
        return "";
    }
    else
    {
        last[client] = res;
    }
    
    /* timestamp */
    XMLElement* timeElem = playerDisconnectionCollectorDoc.NewElement("time");
    timeElem->SetAttribute("time", global.getTime());
    playerDisconnectionCollectorDoc.InsertFirstChild(timeElem);
    
    playerDisconnectionCollectorPrinter.ClearBuffer();
    playerDisconnectionCollectorDoc.Print(&playerDisconnectionCollectorPrinter);

    res = "";

    res += "BEG\n";
    res += playerDisconnectionCollectorPrinter.CStr();
    res += "END";

    return res;
}

void PlayerDisconnectionCollector::clear()
{
    old_clients.clear();
}

void PlayerDisconnectionCollector::clearLast(int client)
{
    last[client] = "";
}

void PlayerDisconnectionCollector::clearAllLast()
{
    for (size_t i = 0; i < last.size(); i++)
    {
        last[i] = "";
    }
}

PlayerDisconnectionCollector::~PlayerDisconnectionCollector() {}
