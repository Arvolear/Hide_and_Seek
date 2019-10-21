#include "../global/convert.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "../player/player.hpp"

#include "playerconnectioncollector.hpp"
        
PlayerConnectionCollector::PlayerConnectionCollector() {}

void PlayerConnectionCollector::collect(vector < int > clients, int playerID)
{
    this->clients = clients;
    this->clients[playerID] = 0;
}

string PlayerConnectionCollector::getData() const
{
    XMLDocument playerConnectionCollectorDoc;

    /* root */
    XMLNode* root = playerConnectionCollectorDoc.NewElement("Con");

    int j = 0;
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i])
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

    playerConnectionCollectorDoc.InsertFirstChild(root);

    /* printer */
    XMLPrinter playerConnectionCollectorPrinter;
    playerConnectionCollectorDoc.Print(&playerConnectionCollectorPrinter);

    string res("BEG\n");
    res += playerConnectionCollectorPrinter.CStr();
    res += "END";

    return res;
}

void PlayerConnectionCollector::clear()
{
    clients.clear();
}

PlayerConnectionCollector::~PlayerConnectionCollector() {}
