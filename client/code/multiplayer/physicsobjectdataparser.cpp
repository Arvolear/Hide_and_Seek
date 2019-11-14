#include "../shader/shader.hpp"

#include "../global/globaluse.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../player/camera.hpp"

#include "../debug/debugdrawer.hpp"

#include "../world/raytracer.hpp"
#include "../world/constrainthandler.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../game_object/sphere.hpp"
#include "../game_object/openglmotionstate.hpp"
#include "../game_object/animation.hpp"
#include "../game_object/mesh.hpp"
#include "../game_object/bone.hpp"
#include "../game_object/skeleton.hpp"
#include "../game_object/viewfrustum.hpp"
#include "../game_object/boundsphere.hpp"
#include "../game_object/modelloader.hpp"
#include "../game_object/physicsobject.hpp"
#include "../game_object/gameobject.hpp"
#include "../game_object/weapon.hpp"
#include "../game_object/rifle.hpp"

#include "physicsobjectdataparser.hpp"

PhysicsObjectDataParser::PhysicsObjectDataParser() {}

void PhysicsObjectDataParser::parse(XMLElement* objElem)
{
    const char* name = nullptr;
    objElem->QueryStringAttribute("name", &name);
    names.push_back(name);

    /* shape */
    XMLElement* shapeElem = objElem->FirstChildElement("shape");

    if (shapeElem)
    {
        const char* type = nullptr;
        shapeElem->QueryStringAttribute("type", &type);

        if (!strcmp(type, "box"))
        {
            float x = 0, y = 0, z = 0;

            shapeElem->QueryFloatAttribute("x", &x);
            shapeElem->QueryFloatAttribute("y", &y);
            shapeElem->QueryFloatAttribute("z", &z);

            collShapes.insert({name, new btBoxShape(btVector3(x, y, z))});    
        }
        else if (!strcmp(type, "cylinder"))
        {
            float radius = 0, height = 0, zup = 0;

            shapeElem->QueryFloatAttribute("r", &radius);
            shapeElem->QueryFloatAttribute("h", &height);
            shapeElem->QueryFloatAttribute("zup", &zup);

            collShapes.insert({name, new btCylinderShape(btVector3(radius, height, zup))});    
        }
        else if (!strcmp(type, "sphere"))
        {
            float radius = 0;

            shapeElem->QueryFloatAttribute("r", &radius);

            collShapes.insert({name, new btSphereShape(radius)});
        }
        else if (!strcmp(type, "capsule"))
        {
            float radius = 0;
            float height = 0;

            shapeElem->QueryFloatAttribute("r", &radius);
            shapeElem->QueryFloatAttribute("h", &height);

            collShapes.insert({name, new btCapsuleShape(radius, height)});
        }
        else if (!strcmp(type, "compound"))
        {
            CompoundShape* CS = new CompoundShape;

            XMLElement* childShapeElem = shapeElem->FirstChildElement("cshape");

            while (childShapeElem)
            {
                const char* childType = nullptr;
                childShapeElem->QueryStringAttribute("type", &childType);

                /* child shape */
                btCollisionShape* childShape = nullptr;

                if (!strcmp(childType, "box"))
                {
                    float x = 0, y = 0, z = 0;

                    childShapeElem->QueryFloatAttribute("x", &x);
                    childShapeElem->QueryFloatAttribute("y", &y);
                    childShapeElem->QueryFloatAttribute("z", &z);

                    childShape = new btBoxShape(btVector3(x, y, z));
                }
                else if (!strcmp(childType, "cylinder"))
                {
                    float radius = 0, height = 0, zup = 0;

                    childShapeElem->QueryFloatAttribute("r", &radius);
                    childShapeElem->QueryFloatAttribute("h", &height);
                    childShapeElem->QueryFloatAttribute("zup", &zup);

                    childShape = new btCylinderShape(btVector3(radius, height, zup));
                }
                else if (!strcmp(childType, "sphere"))
                {
                    float radius = 0;

                    childShapeElem->QueryFloatAttribute("r", &radius);

                    childShape = new btSphereShape(radius);
                }
                else if (!strcmp(childType, "capsule"))
                {
                    float radius = 0;
                    float height = 0;

                    childShapeElem->QueryFloatAttribute("r", &radius);
                    childShapeElem->QueryFloatAttribute("h", &height);

                    childShape = new btCapsuleShape(radius, height);
                }
                else
                {
                    childShapeElem = childShapeElem->NextSiblingElement();
                    continue;
                }

                /* child position */
                btVector3 childPosition = btVector3(0, 0, 0);
                XMLElement* childPositionElem = childShapeElem->FirstChildElement("pos");

                if (childPositionElem)
                {
                    float x = 0, y = 0, z = 0;

                    childPositionElem->QueryFloatAttribute("x", &x);
                    childPositionElem->QueryFloatAttribute("y", &y);
                    childPositionElem->QueryFloatAttribute("z", &z);

                    childPosition = btVector3(x, y, z);
                }

                /* child rotation */
                btQuaternion childRotation = btQuaternion(btVector3(0, 0, 1), 0);
                XMLElement* childRotationElem = childShapeElem->FirstChildElement("rot");

                if (childRotationElem)
                {
                    float x = 0, y = 0, z = 1, angle = 0;

                    childRotationElem->QueryFloatAttribute("x", &x);
                    childRotationElem->QueryFloatAttribute("y", &y);
                    childRotationElem->QueryFloatAttribute("z", &z);
                    childRotationElem->QueryFloatAttribute("angle", &angle);

                    childRotation = btQuaternion(btVector3(x, y, z), global.toRads(angle));
                }

                CS->add(childShape, childPosition, childRotation);

                childShapeElem = childShapeElem->NextSiblingElement();
            }

            compShapes.insert({name, CS});
        }
    }

    /* mass */
    XMLElement* massElem = objElem->FirstChildElement("mass");

    if (massElem)
    {
        float mass = 0;
        massElem->QueryFloatAttribute("mass", &mass);

        masses.insert({name, mass});
    }

    /* angular factor */
    XMLElement* angularElem = objElem->FirstChildElement("afactor");

    if (angularElem)
    {
        float x = 0, y = 0, z = 0;

        angularElem->QueryFloatAttribute("x", &x);
        angularElem->QueryFloatAttribute("y", &y);
        angularElem->QueryFloatAttribute("z", &z);

        aFactors.insert({name, vec3(x, y, z)});
    }

    mat4 model = mat4(1.0);

    /* model */
    XMLElement* modelElem = objElem->FirstChildElement("mdl");

    if (modelElem)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                string str;
                str = char('a' + (i * 4 + j));

                modelElem->QueryFloatAttribute(str.data(), &model[i][j]);
            }
        }
    }

    models.insert({name, model});
}

void PhysicsObjectDataParser::updatePhysicsObject(GameObject* gameObject, bool interpolation, unsigned int timeStamp)
{
    if (names.empty())
    {
        return;
    }

    auto collIt = collShapes.find(gameObject->getName());
    auto compIt = compShapes.find(gameObject->getName());
    auto massIt = masses.find(gameObject->getName());
    auto modelIt = models.find(gameObject->getName());
    auto aFactorIt = aFactors.find(gameObject->getName());

    if (collIt != collShapes.end())
    {
        gameObject->getPhysicsObject()->setShape(collIt->second); 
    }
    else if (compIt != compShapes.end())
    {
        gameObject->getPhysicsObject()->setShape(compIt->second); 
    }

    if (massIt != masses.end())
    {
        gameObject->getPhysicsObject()->setMass(massIt->second, false); 
    }

    if (modelIt != models.end())
    {
        gameObject->setPhysicsObjectTransform(modelIt->second, interpolation, timeStamp); 
    }

    if (aFactorIt != aFactors.end())
    {
        gameObject->getPhysicsObject()->getRigidBody()->setAngularFactor(global.toBtVector3(aFactorIt->second)); 
    }
}

vector < string > PhysicsObjectDataParser::getNames() const
{
    return names;
}

void PhysicsObjectDataParser::clear()
{
    names.clear();
    collShapes.clear();
    compShapes.clear();
    masses.clear();
    models.clear();
}

PhysicsObjectDataParser::~PhysicsObjectDataParser() {}
