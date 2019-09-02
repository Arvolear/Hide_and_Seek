#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "../world/raytracer.hpp"

#include "physicsobjectdatacollector.hpp"
        
PhysicsObjectDataCollector::PhysicsObjectDataCollector()
{
    name = "";
    model = new btScalar[16];
    memset(model, 0, 16 * sizeof(btScalar));
}

void PhysicsObjectDataCollector::collect(PhysicsObject* physicsObject)
{
    if (physicsObject)
    {
        model = physicsObject->getTransform();
        name = physicsObject->getName();
    }
}

string PhysicsObjectDataCollector::getData() const
{
    XMLDocument physicsObjectDataCollectorDoc;

    /* root */
    XMLNode* root = physicsObjectDataCollectorDoc.NewElement("Obj");

    physicsObjectDataCollectorDoc.InsertFirstChild(root);

    /* name */
    XMLElement* nameElem = physicsObjectDataCollectorDoc.NewElement("name");
    nameElem->SetText(name.data());

    root->InsertEndChild(nameElem);

    /* model */
    XMLElement* modelElem = physicsObjectDataCollectorDoc.NewElement("mdl");

    for (int i = 0; i < 16; i++)
    {
        string str;
        str = char('a' + i);

        modelElem->SetAttribute(str.data(), model[i]);
    }

    root->InsertEndChild(modelElem);

    /* printer */
    XMLPrinter physicsObjectDataCollectorPrinter;
    physicsObjectDataCollectorDoc.Print(&physicsObjectDataCollectorPrinter);

    string res("BEG\n");
    res += physicsObjectDataCollectorPrinter.CStr();
    res += "END";

    return res;
}
        
void PhysicsObjectDataCollector::clear()
{
    name = "";
    memset(model, 0, 16 * sizeof(btScalar));
}

PhysicsObjectDataCollector::~PhysicsObjectDataCollector() 
{
    delete[] model;
}
