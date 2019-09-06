#include "../global/convert.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "physicsobjectdataupdater.hpp"

PhysicsObjectDataUpdater::PhysicsObjectDataUpdater()
{
    name = "";
    senderID = -1;

    model = new btScalar[16];
    memset(model, 0, sizeof(btScalar) * 16);
}

void PhysicsObjectDataUpdater::collect(string info)
{
    XMLDocument physicsObjectDataUpdaterDoc;
    
    physicsObjectDataUpdaterDoc.Parse(info.data());

    /* root */
    XMLNode* root = physicsObjectDataUpdaterDoc.FirstChildElement("Obj");

    if (!root)
    {
        physicsObjectDataUpdaterDoc.PrintError();
        cout << info << endl << endl;
        throw runtime_error("ERROR::PhysicsObjectDataUpdater::collect() failed to load XML");
    }

    /* name */
    XMLElement* nameElem = root->FirstChildElement("name");
    
    if (nameElem)
    {
        name = nameElem->GetText();
    }
    
    /* senderID */
    XMLElement* senderIDElem = root->FirstChildElement("id");
    
    if (senderIDElem)
    {
        senderIDElem->QueryIntText(&senderID);
    }

    /* model */
    XMLElement* modelElem = root->FirstChildElement("mdl");

    if (modelElem)
    {
        for (int i = 0; i < 16; i++)
        {
            string str;
            str = char('a' + i);

            modelElem->QueryFloatAttribute(str.data(), &model[i]);
        }
    }
}

void PhysicsObjectDataUpdater::updateData(PhysicsObject* physicsObject)
{
    physicsObject->setTransform(model);
}

string PhysicsObjectDataUpdater::getName() const
{
    return name;
}
        
int PhysicsObjectDataUpdater::getSenderID() const
{
    return senderID;
}

void PhysicsObjectDataUpdater::clear()
{
    name = "";
    senderID = -1;

    memset(model, 0, sizeof(btScalar) * 16);
}

PhysicsObjectDataUpdater::~PhysicsObjectDataUpdater() 
{
    delete[] model;
}
