#include "../global/convert.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"

#include "../player/player.hpp"

#include "levelloader.hpp"

LevelLoader::LevelLoader(World* physicsWorld)
{
    this->physicsWorld = physicsWorld;
}
        
void LevelLoader::loadPhysicsObject(XMLElement* physicsObjectElem, PhysicsObject*& PO)
{
    /* name */
    const char* name = nullptr;

    physicsObjectElem->QueryStringAttribute("name", &name);

    PO = new PhysicsObject(name, physicsWorld->getWorld());

    /* shape */
    XMLElement* shapeElem = physicsObjectElem->FirstChildElement("shape");

    if (shapeElem)
    {
        bool shape = false;

        const char* type = nullptr;
        shapeElem->QueryStringAttribute("type", &type);

        if (!strcmp(type, "box"))
        {
            float x = 0, y = 0, z = 0;

            shapeElem->QueryFloatAttribute("x", &x);
            shapeElem->QueryFloatAttribute("y", &y);
            shapeElem->QueryFloatAttribute("z", &z);

            PO->setShape(new btBoxShape(btVector3(x, y, z)));    
            shape = true;
        }
        else if (!strcmp(type, "cylinder"))
        {
            float radius = 0, height = 0, zup = 0;

            shapeElem->QueryFloatAttribute("radius", &radius);
            shapeElem->QueryFloatAttribute("height", &height);
            shapeElem->QueryFloatAttribute("zup", &zup);

            PO->setShape(new btCylinderShape(btVector3(radius, height, zup)));    
            shape = true;
        }
        else if (!strcmp(type, "sphere"))
        {
            float radius = 0;

            shapeElem->QueryFloatAttribute("radius", &radius);

            PO->setShape(new btSphereShape(radius));    
            shape = true;
        }
        else if (!strcmp(type, "capsule"))
        {
            float radius = 0;
            float height = 0;

            shapeElem->QueryFloatAttribute("radius", &radius);
            shapeElem->QueryFloatAttribute("height", &height);

            PO->setShape(new btCapsuleShape(radius, height));    
            shape = true;
        }
        else if (!strcmp(type, "compound"))
        {
            CompoundShape* CS = new CompoundShape;

            XMLElement* childShapeElem = shapeElem->FirstChildElement("childshape");

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

                    childShapeElem->QueryFloatAttribute("radius", &radius);
                    childShapeElem->QueryFloatAttribute("height", &height);
                    childShapeElem->QueryFloatAttribute("zup", &zup);

                    childShape = new btCylinderShape(btVector3(radius, height, zup));
                }
                else if (!strcmp(childType, "sphere"))
                {
                    float radius = 0;

                    childShapeElem->QueryFloatAttribute("radius", &radius);

                    childShape = new btSphereShape(radius);
                }
                else if (!strcmp(childType, "capsule"))
                {
                    float radius = 0;
                    float height = 0;

                    childShapeElem->QueryFloatAttribute("radius", &radius);
                    childShapeElem->QueryFloatAttribute("height", &height);

                    childShape = new btCapsuleShape(radius, height);
                }
                else
                {
                    childShapeElem = childShapeElem->NextSiblingElement();
                    continue;
                }

                /* child position */
                btVector3 childPosition = btVector3(0, 0, 0);
                XMLElement* childPositionElem = childShapeElem->FirstChildElement("position");

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
                XMLElement* childRotationElem = childShapeElem->FirstChildElement("rotation");

                if (childRotationElem)
                {
                    float x = 0, y = 0, z = 1, angle = 0;

                    childRotationElem->QueryFloatAttribute("x", &x);
                    childRotationElem->QueryFloatAttribute("y", &y);
                    childRotationElem->QueryFloatAttribute("z", &z);
                    childRotationElem->QueryFloatAttribute("angle", &angle);

                    childRotation = btQuaternion(btVector3(x, y, z), toRads(angle));
                }

                CS->add(childShape, childPosition, childRotation);
                shape = true;

                childShapeElem = childShapeElem->NextSiblingElement();
            }

            PO->setShape(CS);    
        }

        if (shape)
        {
            /* mass */
            XMLElement* massElem = physicsObjectElem->FirstChildElement("mass");

            if (massElem)
            {
                float mass = 0;
                massElem->QueryFloatAttribute("mass", &mass);

                PO->setMass(mass);
            }

            /* position */
            XMLElement* positionElem = physicsObjectElem->FirstChildElement("position");

            if (positionElem)
            {
                float x = 0, y = 0, z = 0;

                positionElem->QueryFloatAttribute("x", &x);
                positionElem->QueryFloatAttribute("y", &y);
                positionElem->QueryFloatAttribute("z", &z);

                PO->setPosition(btVector3(x, y, z));
            }

            /* rotation */
            XMLElement* rotationElem = physicsObjectElem->FirstChildElement("rotation");

            if (rotationElem)
            {
                float x = 0, y = 0, z = 0, angle = 0;

                rotationElem->QueryFloatAttribute("x", &x);
                rotationElem->QueryFloatAttribute("y", &y);
                rotationElem->QueryFloatAttribute("z", &z);
                rotationElem->QueryFloatAttribute("angle", &angle);

                PO->setRotation(btQuaternion(btVector3(x, y, z), toRads(angle)));
            }

            /* angular factor */
            XMLElement* angularElem = physicsObjectElem->FirstChildElement("angularfactor");

            if (angularElem)
            {
                float x = 0, y = 0, z = 0;

                angularElem->QueryFloatAttribute("x", &x);
                angularElem->QueryFloatAttribute("y", &y);
                angularElem->QueryFloatAttribute("z", &z);

                PO->getRigidBody()->setAngularFactor(btVector3(x, y, z));
            }
        }
    }
}

void LevelLoader::loadPhysicsObjects()
{
    XMLDocument physicsObjectDoc;

    physicsObjectDoc.LoadFile((levelName + "/physics_object.xml").c_str());

    XMLNode* root = physicsObjectDoc.FirstChildElement("PhysicsObjectFile");

    if (!root)
    {
        throw runtime_error("ERROR::loadPhysicsObjects() failed to load XML");
    }

    XMLNode* physicsObjectsNode = root->FirstChildElement("physicsobjects"); 
    XMLElement* physicsObjectElem = physicsObjectsNode->FirstChildElement("physicsobject");
    
    while (physicsObjectElem)
    {
        PhysicsObject* PO = nullptr;
        loadPhysicsObject(physicsObjectElem, PO);

        physicsObjects.insert({PO->getName(), PO});

        physicsObjectElem = physicsObjectElem->NextSiblingElement();
    }
}

void LevelLoader::loadPlayers()
{
    XMLDocument playerDoc;

    playerDoc.LoadFile((levelName + "/player.xml").c_str());

    XMLNode* root = playerDoc.FirstChildElement("PlayerFile");

    if (!root)
    {
        throw runtime_error("ERROR::loadPlayers() failed to load XML");
    }

    XMLNode* playerNode = root->FirstChildElement("players"); 
    XMLElement* playerElem = playerNode->FirstChildElement("player");

    while (playerElem)
    {
        /* speed */
        XMLElement* speedElem = playerElem->FirstChildElement("speed");
        float speed = 1.0;

        if (speedElem)
        {
            speedElem->QueryFloatAttribute("speed", &speed);
        }

        Player* player = new Player(speed);

        /* game object */
        XMLElement* physicsObjectElem = playerElem->FirstChildElement("physicsobject");

        if (physicsObjectElem)
        {
            PhysicsObject* PO = nullptr;
            loadPhysicsObject(physicsObjectElem, PO);

            physicsObjects.insert({PO->getName(), PO});

            player->setPhysicsObject(PO);
        }

        /* raytracer */
        XMLElement* rayTracerElem = playerElem->FirstChildElement("raytracer");

        if (rayTracerElem)
        {
            const char* apply = nullptr;

            rayTracerElem->QueryStringAttribute("apply", &apply);

            if (!strcmp("true", apply))
            {
                RayTracer* rayTracer = new RayTracer(physicsWorld->getWorld());     

                player->setRayTracer(rayTracer);
            }
        }

        players.push_back(player);

        playerElem = playerElem->NextSiblingElement();
    }

    if (players.empty())
    {
        throw runtime_error("ERROR::loadPlayers() at least 1 player is required");
    }
}

void LevelLoader::loadLevel(string name)
{
    this->levelName = name;

    loadPhysicsObjects();

    loadPlayers();
}

void LevelLoader::getPhysicsObjectsData(map < string, PhysicsObject* > &physicsObjects) const
{
    physicsObjects = this->physicsObjects;
}

void LevelLoader::getPlayersData(vector < Player* > &players) const
{
    players = this->players;
}

LevelLoader::~LevelLoader() {}
