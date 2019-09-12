#include "../global/convert.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "../world/raytracer.hpp"

#include "../player/player.hpp"

#include "playerdisconnectioncollector.hpp"
        
PlayerDisconnectionCollector::PlayerDisconnectionCollector() {}

void PlayerDisconnectionCollector::collect(vector < int > old_clients)
{
    this->old_clients = old_clients;
}

string PlayerDisconnectionCollector::getData() const
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
        return "";
    }

    playerDisconnectionCollectorDoc.InsertFirstChild(root);

    /* printer */
    XMLPrinter playerDisconnectionCollectorPrinter;
    playerDisconnectionCollectorDoc.Print(&playerDisconnectionCollectorPrinter);

    string res("BEG\n");
    res += playerDisconnectionCollectorPrinter.CStr();
    res += "END";

    return res;
}

void PlayerDisconnectionCollector::clear()
{
    old_clients.clear();
}

PlayerDisconnectionCollector::~PlayerDisconnectionCollector() {}
