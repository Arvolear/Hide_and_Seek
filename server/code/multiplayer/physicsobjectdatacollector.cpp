#include "../global/convert.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "../world/raytracer.hpp"

#include "physicsobjectdatacollector.hpp"
        
PhysicsObjectDataCollector::PhysicsObjectDataCollector() {}

void PhysicsObjectDataCollector::collect(PhysicsObject* physicsObject)
{
    if (physicsObject)
    {
        models.push_back(physicsObject->getTransform());
        names.push_back(physicsObject->getName());
    }
}

void PhysicsObjectDataCollector::collect(map < string, PhysicsObject* > physicsObjects)
{
    for (auto& i: physicsObjects)
    {
        models.push_back(i.second->getTransform());
        names.push_back(i.second->getName());
    }
}

string PhysicsObjectDataCollector::getData(bool raw) const
{
    XMLDocument physicsObjectDataCollectorDoc;

    /* root */
    XMLNode* root = physicsObjectDataCollectorDoc.NewElement("Objs");
    physicsObjectDataCollectorDoc.InsertEndChild(root);

    for (size_t i = 0; i < names.size(); i++)
    {
        /* obj */
        XMLNode* objElem = physicsObjectDataCollectorDoc.NewElement("obj");

        /* name */
        XMLElement* nameElem = physicsObjectDataCollectorDoc.NewElement("name");
        nameElem->SetText(names[i].data());

        objElem->InsertEndChild(nameElem);

        /* model */
        XMLElement* modelElem = physicsObjectDataCollectorDoc.NewElement("mdl");

        for (int j = 0; j < 16; j++)
        {
            string str;
            str = char('a' + j);

            modelElem->SetAttribute(str.data(), cutFloat(models[i][j], 4));
        }

        objElem->InsertEndChild(modelElem);

        root->InsertEndChild(objElem);
    }

    /* printer */
    XMLPrinter physicsObjectDataCollectorPrinter;
    physicsObjectDataCollectorDoc.Print(&physicsObjectDataCollectorPrinter);

    string res;

    if (!raw)
    {
        res = "BEG\n";
        res += physicsObjectDataCollectorPrinter.CStr();
        res += "END";
    }
    else
    {
        res = physicsObjectDataCollectorPrinter.CStr();
    }

    return res;
}

void PhysicsObjectDataCollector::clear()
{
    names.clear();

    for (size_t i = 0; i < models.size(); i++)
    {
        delete[] models[i];
    }

    models.clear();
}

PhysicsObjectDataCollector::~PhysicsObjectDataCollector() 
{
    for (size_t i = 0; i < models.size(); i++)
    {
        delete[] models[i];
    }
}
