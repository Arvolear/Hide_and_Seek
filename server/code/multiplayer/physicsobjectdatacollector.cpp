#include "../global/globaluse.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "physicsobjectdatacollector.hpp"
        
PhysicsObjectDataCollector::PhysicsObjectDataCollector(int clients) 
{
    last.resize(clients, "");
}

void PhysicsObjectDataCollector::collect(PhysicsObject* physicsObject)
{
    if (physicsObject)
    {
        pos.insert({physicsObject->getName(), physicsObject->getTransform()});
    }
}

void PhysicsObjectDataCollector::collect(map < string, PhysicsObject* > physicsObjects)
{
    for (auto& i: physicsObjects)
    {
        pos.insert({i.first, i.second->getTransform()});
    }
}

string PhysicsObjectDataCollector::getData(int client, bool raw) const
{
    XMLDocument physicsObjectDataCollectorDoc;

    /* root */
    XMLNode* root = physicsObjectDataCollectorDoc.NewElement("Objs");
    physicsObjectDataCollectorDoc.InsertFirstChild(root);
    
    for (auto& i: pos)
    {
        /* obj */
        XMLElement* objElem = physicsObjectDataCollectorDoc.NewElement("obj");

        /* name */
        objElem->SetAttribute("name", i.first.data());

        /* model */
        XMLElement* modelElem = physicsObjectDataCollectorDoc.NewElement("mdl");

        for (int j = 0; j < 16; j++)
        {
            string str;
            str = char('a' + j);

            modelElem->SetAttribute(str.data(), global.cutFloat(i.second[j], 4));
        }

        objElem->InsertEndChild(modelElem);

        root->InsertEndChild(objElem);
    }

    /* printer */
    XMLPrinter physicsObjectDataCollectorPrinter;
    physicsObjectDataCollectorDoc.Print(&physicsObjectDataCollectorPrinter);
    
    string res = physicsObjectDataCollectorPrinter.CStr();
    
    if (res == last[client])
    {
        return "";
    }
    else
    {
        last[client] = res;
    }
    
    /* timestamp */
    XMLElement* timeElem = physicsObjectDataCollectorDoc.NewElement("time");
    timeElem->SetAttribute("time", global.getTime());
    physicsObjectDataCollectorDoc.InsertFirstChild(timeElem);
    
    physicsObjectDataCollectorPrinter.ClearBuffer();
    physicsObjectDataCollectorDoc.Print(&physicsObjectDataCollectorPrinter);

    res = "";

    if (!raw)
    {
        res += "BEG\n";
        res += physicsObjectDataCollectorPrinter.CStr();
        res += "END";
    }
    else
    {
        res += physicsObjectDataCollectorPrinter.CStr();
    }

    return res;
}
        
string PhysicsObjectDataCollector::getMergedData(string fileName, int client, bool raw) const
{
    XMLDocument physicsObjectDataCollectorDoc;

    physicsObjectDataCollectorDoc.LoadFile(fileName.c_str());

    XMLNode* root = physicsObjectDataCollectorDoc.FirstChildElement("Objs");

    if (!root)
    {
        throw runtime_error("ERROR::PhysicsObjectDataCollector::getMergedData() failed to load XML");
    }
    
    XMLElement* objElem = root->FirstChildElement("obj");

    while (objElem)
    {
        const char* name = nullptr;
        objElem->QueryStringAttribute("name", &name);

        XMLElement* positionElem = objElem->FirstChildElement("pos");
        
        if (positionElem)
        {
            objElem->DeleteChild(positionElem);
        }

        XMLElement* rotationElem = objElem->FirstChildElement("rot");

        while (rotationElem && !strcmp(rotationElem->Value(), "rot"))
        {
            objElem->DeleteChild(rotationElem);
            rotationElem = rotationElem->NextSiblingElement();
        }

        /* model */
        XMLElement* modelElem = physicsObjectDataCollectorDoc.NewElement("mdl");

        btScalar* model = pos[name];

        if (model)
        {
            for (int j = 0; j < 16; j++)
            {
                string str;
                str = char('a' + j);

                modelElem->SetAttribute(str.data(), global.cutFloat(model[j], 4));
            }

            objElem->InsertEndChild(modelElem);
        }

        objElem = objElem->NextSiblingElement();
    }

    /* printer */
    XMLPrinter physicsObjectDataCollectorPrinter;
    physicsObjectDataCollectorDoc.Print(&physicsObjectDataCollectorPrinter);
    
    string res = physicsObjectDataCollectorPrinter.CStr();
    
    if (res == last[client])
    {
        return "";
    }
    else
    {
        last[client] = res;
    }
    
    /* timestamp */
    XMLElement* timeElem = physicsObjectDataCollectorDoc.NewElement("time");
    timeElem->SetAttribute("time", global.getTime());
    physicsObjectDataCollectorDoc.InsertFirstChild(timeElem);
    
    physicsObjectDataCollectorPrinter.ClearBuffer();
    physicsObjectDataCollectorDoc.Print(&physicsObjectDataCollectorPrinter);

    res = "";

    if (!raw)
    {
        res += "BEG\n";
        res += physicsObjectDataCollectorPrinter.CStr();
        res += "END";
    }
    else
    {
        res += physicsObjectDataCollectorPrinter.CStr();
    }

    return res;
}

void PhysicsObjectDataCollector::clear()
{
    for (auto& i: pos)
    {
        delete[] i.second;
    }

    pos.clear();
}

void PhysicsObjectDataCollector::clearLast(int client)
{
    last[client] = "";
}
        
void PhysicsObjectDataCollector::clearAllLast()
{
    for (size_t i = 0; i < last.size(); i++)
    {
        last[i] = "";
    }
}

PhysicsObjectDataCollector::~PhysicsObjectDataCollector() 
{
    clear();
}
