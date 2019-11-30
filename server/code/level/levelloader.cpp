#include "../global/globaluse.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "spawner.hpp"
#include "levelloader.hpp"

LevelLoader::LevelLoader(World* physicsWorld)
{
    this->physicsWorld = physicsWorld;
}
        
void LevelLoader::loadPhysicsObject(XMLElement* physicsObjectElem, PhysicsObject*& PO, string name)
{
    if (!PO)
    {
        PO = new PhysicsObject(name, physicsWorld);
    }

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

            shapeElem->QueryFloatAttribute("r", &radius);
            shapeElem->QueryFloatAttribute("h", &height);
            shapeElem->QueryFloatAttribute("zup", &zup);

            PO->setShape(new btCylinderShape(btVector3(radius, height, zup)));    
            shape = true;
        }
        else if (!strcmp(type, "sphere"))
        {
            float radius = 0;

            shapeElem->QueryFloatAttribute("r", &radius);

            PO->setShape(new btSphereShape(radius));    
            shape = true;
        }
        else if (!strcmp(type, "capsule"))
        {
            float radius = 0;
            float height = 0;

            shapeElem->QueryFloatAttribute("r", &radius);
            shapeElem->QueryFloatAttribute("h", &height);

            PO->setShape(new btCapsuleShape(radius, height));    
            shape = true;
        }
        else if (!strcmp(type, "convex"))
        {
            ConvexHullShape* CHS = new ConvexHullShape();

            XMLElement* pointElem = shapeElem->FirstChildElement("point");

            while (pointElem)
            {
                float x = 0, y = 0, z = 0;

                pointElem->QueryFloatAttribute("x", &x);
                pointElem->QueryFloatAttribute("y", &y);
                pointElem->QueryFloatAttribute("z", &z);

                CHS->addPoint(btVector3(x, y, z));

                pointElem = pointElem->NextSiblingElement();
            }

            PO->setShape(CHS);
            shape = true;
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
                else if (!strcmp(childType, "convex"))
                {
                    ConvexHullShape* CHS = new ConvexHullShape();

                    XMLElement* pointElem = childShapeElem->FirstChildElement("point");

                    while (pointElem)
                    {
                        float x = 0, y = 0, z = 0;

                        pointElem->QueryFloatAttribute("x", &x);
                        pointElem->QueryFloatAttribute("y", &y);
                        pointElem->QueryFloatAttribute("z", &z);

                        CHS->addPoint(btVector3(x, y, z));

                        pointElem = pointElem->NextSiblingElement();
                    }

                    childShape = CHS->getShape();

                    delete CHS;
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

                PO->setMass(mass, false);
            }

            /* position */
            XMLElement* positionElem = physicsObjectElem->FirstChildElement("pos");

            if (positionElem)
            {
                float x = 0, y = 0, z = 0;

                positionElem->QueryFloatAttribute("x", &x);
                positionElem->QueryFloatAttribute("y", &y);
                positionElem->QueryFloatAttribute("z", &z);

                PO->setPosition(btVector3(x, y, z), false);
            }

            /* rotation */
            XMLElement* rotationElem = physicsObjectElem->FirstChildElement("rot");
            bool add = false;

            while (rotationElem && !strcmp(rotationElem->Value(), "rot"))
            {
                float x = 0, y = 0, z = 0, angle = 0;

                rotationElem->QueryFloatAttribute("x", &x);
                rotationElem->QueryFloatAttribute("y", &y);
                rotationElem->QueryFloatAttribute("z", &z);
                rotationElem->QueryFloatAttribute("angle", &angle);

                PO->setRotation(btQuaternion(btVector3(x, y, z), global.toRads(angle)), add);
                add = true;

                rotationElem = rotationElem->NextSiblingElement();
            }

            /* angular factor */
            XMLElement* angularElem = physicsObjectElem->FirstChildElement("afactor");

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

void LevelLoader::loadWeapon(XMLElement* weaponElem, Weapon*& WE, string name)
{
    if (!WE)
    {
        WE = new Weapon(name, physicsWorld);
    }

    /* shape */
    XMLElement* shapeElem = weaponElem->FirstChildElement("shape");

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

            WE->setShape(new btBoxShape(btVector3(x, y, z)));    
            shape = true;
        }
        else if (!strcmp(type, "cylinder"))
        {
            float radius = 0, height = 0, zup = 0;

            shapeElem->QueryFloatAttribute("r", &radius);
            shapeElem->QueryFloatAttribute("h", &height);
            shapeElem->QueryFloatAttribute("zup", &zup);

            WE->setShape(new btCylinderShape(btVector3(radius, height, zup)));    
            shape = true;
        }
        else if (!strcmp(type, "sphere"))
        {
            float radius = 0;

            shapeElem->QueryFloatAttribute("r", &radius);

            WE->setShape(new btSphereShape(radius));    
            shape = true;
        }
        else if (!strcmp(type, "capsule"))
        {
            float radius = 0;
            float height = 0;

            shapeElem->QueryFloatAttribute("r", &radius);
            shapeElem->QueryFloatAttribute("h", &height);

            WE->setShape(new btCapsuleShape(radius, height));    
            shape = true;
        }
        else if (!strcmp(type, "convex"))
        {
            ConvexHullShape* CHS = new ConvexHullShape();

            XMLElement* pointElem = shapeElem->FirstChildElement("point");

            while (pointElem)
            {
                float x = 0, y = 0, z = 0;

                pointElem->QueryFloatAttribute("x", &x);
                pointElem->QueryFloatAttribute("y", &y);
                pointElem->QueryFloatAttribute("z", &z);

                CHS->addPoint(btVector3(x, y, z));

                shape = true;
                pointElem = pointElem->NextSiblingElement();
            }

            WE->setShape(CHS);
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
                else if (!strcmp(childType, "convex"))
                {
                    ConvexHullShape* CHS = new ConvexHullShape();

                    XMLElement* pointElem = childShapeElem->FirstChildElement("point");

                    while (pointElem)
                    {
                        float x = 0, y = 0, z = 0;

                        pointElem->QueryFloatAttribute("x", &x);
                        pointElem->QueryFloatAttribute("y", &y);
                        pointElem->QueryFloatAttribute("z", &z);

                        CHS->addPoint(btVector3(x, y, z));

                        pointElem = pointElem->NextSiblingElement();
                    }

                    childShape = CHS->getShape();

                    delete CHS;
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
                shape = true;

                childShapeElem = childShapeElem->NextSiblingElement();
            }

            WE->setShape(CS);    
        }

        if (shape)
        {
            /* mass */
            XMLElement* massElem = weaponElem->FirstChildElement("mass");

            if (massElem)
            {
                float mass = 0;
                massElem->QueryFloatAttribute("mass", &mass);

                WE->setMass(mass, false);
            }

            /* position */
            XMLElement* positionElem = weaponElem->FirstChildElement("pos");

            if (positionElem)
            {
                float x = 0, y = 0, z = 0;

                positionElem->QueryFloatAttribute("x", &x);
                positionElem->QueryFloatAttribute("y", &y);
                positionElem->QueryFloatAttribute("z", &z);

                WE->setPosition(btVector3(x, y, z), false);
            }

            /* rotation */
            XMLElement* rotationElem = weaponElem->FirstChildElement("rot");
            bool add = false;

            while (rotationElem && !strcmp(rotationElem->Value(), "rot"))
            {
                float x = 0, y = 0, z = 0, angle = 0;

                rotationElem->QueryFloatAttribute("x", &x);
                rotationElem->QueryFloatAttribute("y", &y);
                rotationElem->QueryFloatAttribute("z", &z);
                rotationElem->QueryFloatAttribute("angle", &angle);

                WE->setRotation(btQuaternion(btVector3(x, y, z), global.toRads(angle)), add);
                add = true;

                rotationElem = rotationElem->NextSiblingElement();
            }

            /* angular factor */
            XMLElement* angularElem = weaponElem->FirstChildElement("afactor");

            if (angularElem)
            {
                float x = 0, y = 0, z = 0;

                angularElem->QueryFloatAttribute("x", &x);
                angularElem->QueryFloatAttribute("y", &y);
                angularElem->QueryFloatAttribute("z", &z);

                WE->getRigidBody()->setAngularFactor(btVector3(x, y, z));
            }
        }
    }

    XMLElement* weaponInfoElem = weaponElem->FirstChildElement("info");

    /* info */
    if (weaponElem)
    {
        /* storage bullets */
        XMLElement* storageBulletsElem = weaponInfoElem->FirstChildElement("storage");

        if (storageBulletsElem)
        {
            int amount = 0;

            storageBulletsElem->QueryIntAttribute("amount", &amount);

            WE->setStorageBullets(amount);
        }

        /* magazine size */
        XMLElement* magazineSizeElem = weaponInfoElem->FirstChildElement("size");

        if (magazineSizeElem)
        {
            int size = 0;

            magazineSizeElem->QueryIntAttribute("size", &size);

            WE->setMagazineSize(size);
        }

        /* magazine bullets */
        XMLElement* magazineBulletsElem = weaponInfoElem->FirstChildElement("magazine");

        if (magazineBulletsElem)
        {
            int amount = 0;

            magazineBulletsElem->QueryIntAttribute("amount", &amount);

            WE->setMagazineBullets(amount);
        }

        /* shot speed */
        XMLElement* shotSpeedElem = weaponInfoElem->FirstChildElement("speed");

        if (shotSpeedElem)
        {
            float speed = 0;

            shotSpeedElem->QueryFloatAttribute("speed", &speed);

            WE->setShotSpeed(speed);
        }

        /* shot power */
        XMLElement* shotPowerElem = weaponInfoElem->FirstChildElement("power");

        if (shotPowerElem)
        {
            int power = 0;

            shotPowerElem->QueryIntAttribute("power", &power);

            WE->setShotPower(power);
        }
    }
}

void LevelLoader::loadPhysicsObjects()
{
    XMLDocument physicsObjectDoc;

    physicsObjectDoc.LoadFile((levelName + "/physics_object.xml").c_str());

    XMLNode* root = physicsObjectDoc.FirstChildElement("Objs");

    if (!root)
    {
        throw runtime_error("ERROR::loadPhysicsObjects() failed to load XML");
    }

    XMLElement* physicsObjectElem = root->FirstChildElement("obj");

    while (physicsObjectElem)
    {
        const char* baseName = nullptr;
        physicsObjectElem->QueryStringAttribute("name", &baseName);

        int quantity = 1;
        physicsObjectElem->QueryIntAttribute("quantity", &quantity);

        for (int i = 0; i < quantity; i++)
        {
            string name = baseName;
            
            if (quantity > 1)
            {
                name += to_string(i);
            }

            /* new */
            if (physicsObjects.find(name) == physicsObjects.end())
            {
                PhysicsObject* PO = nullptr;

                loadPhysicsObject(physicsObjectElem, PO, name);

                physicsObjects.insert({PO->getName(), PO});
            }
            else // update
            {
                PhysicsObject* PO = physicsObjects.find(name)->second;

                loadPhysicsObject(physicsObjectElem, PO, name);
            }
        }

        physicsObjectElem = physicsObjectElem->NextSiblingElement();
    }
}

void LevelLoader::loadWeapons()
{
    XMLDocument weaponDoc;

    weaponDoc.LoadFile((levelName + "/weapon.xml").c_str());

    XMLNode* root = weaponDoc.FirstChildElement("Weapons");

    if (!root)
    {
        throw runtime_error("ERROR::LevelLoader::loadWeapons() failed to load XML");
    }

    XMLElement* weaponElem = root->FirstChildElement("weapon");

    while (weaponElem)
    {
        const char* baseName = nullptr;
        weaponElem->QueryStringAttribute("name", &baseName);

        int quantity = 1;
        weaponElem->QueryIntAttribute("quantity", &quantity);

        for (int i = 0; i < quantity; i++)
        {
            string name = baseName;

            if (quantity > 1)
            {
                name += to_string(i);
            }

            /* new */
            if (physicsObjects.find(name) == physicsObjects.end())
            {
                Weapon* WE = nullptr;

                loadWeapon(weaponElem, WE, name);

                physicsObjects.insert({WE->getName(), WE});

            }
            else // update
            {
                Weapon* WE = dynamic_cast < Weapon* >(physicsObjects.find(name)->second);

                loadWeapon(weaponElem, WE, name);
            }
        }

        weaponElem = weaponElem->NextSiblingElement();
    } 
}

void LevelLoader::loadSoldiers()
{
    XMLDocument soldierDoc;

    soldierDoc.LoadFile((levelName + "/soldier.xml").c_str());

    XMLNode* root = soldierDoc.FirstChildElement("Soldiers");

    if (!root)
    {
        throw runtime_error("ERROR::LevelLoader::loadSoldiers() failed to load XML");
    }

    XMLElement* soldierElem = root->FirstChildElement("soldier");

    while (soldierElem)
    {
        /* id */
        int id = -1;
        soldierElem->QueryIntAttribute("id", &id);

        /* speed */
        XMLElement* speedElem = soldierElem->FirstChildElement("speed");
        float speed = 1.0;

        if (speedElem)
        {
            speedElem->QueryFloatAttribute("speed", &speed);
        }
        
        /* health */
        XMLElement* healthElem = soldierElem->FirstChildElement("health");
        int health = 1.0;

        if (healthElem)
        {
            healthElem->QueryIntAttribute("health", &health);
        }

        Soldier* soldier = nullptr;

        bool found = false;
        for (size_t i = 0; i < players.size(); i++)
        {
            if (players[i]->getID() == id)
            {
                found = true;
                soldier = dynamic_cast < Soldier* >(players[i]);
                break;
            }
        }

        if (!found)
        {
            soldier = new Soldier(health, id, speed);
            players.push_back(soldier);
        }

        /* physics object */
        XMLElement* physicsObjectElem = soldierElem->FirstChildElement("obj");

        if (physicsObjectElem)
        {
            const char* name = nullptr;
            physicsObjectElem->QueryStringAttribute("name", &name);

            /* new */
            if (physicsObjects.find(name) == physicsObjects.end())
            {
                PhysicsObject* PO = nullptr;

                loadPhysicsObject(physicsObjectElem, PO, name);

                physicsObjects.insert({PO->getName(), PO});

                soldier->setPhysicsObject(PO);
            }
            else // update
            {
                PhysicsObject* PO = physicsObjects.find(name)->second;

                loadPhysicsObject(physicsObjectElem, PO, name);
            }
        }

        if (!found)
        {
            /* add weapon */
            XMLElement* armoryElem = soldierElem->FirstChildElement("armory");

            if (armoryElem)
            {
                XMLElement* weaponElem = armoryElem->FirstChildElement("weapon");

                while (weaponElem)
                {
                    const char* name = nullptr;
                    weaponElem->QueryStringAttribute("name", &name);

                    Weapon* weapon = dynamic_cast < Weapon* >(physicsObjects.find(name)->second);

                    if (!weapon)
                    {
                        throw runtime_error("ERROR::LevelLoader::loadSoldiers() can't find the weapon");
                    }

                    soldier->pick(weapon);

                    weaponElem = weaponElem->NextSiblingElement();
                }
            }
        }

        soldierElem = soldierElem->NextSiblingElement();
    }
}
        
void LevelLoader::loadSpawner()
{
    XMLDocument spawnerDoc;

    spawnerDoc.LoadFile((levelName + "/spawn.xml").c_str());

    XMLNode* root = spawnerDoc.FirstChildElement("Spawner");

    if (!root)
    {
        throw runtime_error("ERROR::LevelLoader::loadSpawner() failed to load XML");
    }

    spawner = new Spawner(players.size());

    XMLElement* posElem = root->FirstChildElement("pos");

    while (posElem)
    {
        float x = 0.0, y = 0.0, z = 0.0;

        posElem->QueryFloatAttribute("x", &x);
        posElem->QueryFloatAttribute("y", &y);
        posElem->QueryFloatAttribute("z", &z);

        spawner->addPosition(btVector3(x, y, z));

        posElem = posElem->NextSiblingElement();
    }
}

void LevelLoader::loadLevel(string name)
{
    this->levelName = name;

    loadPhysicsObjects();
    loadWeapons();

    loadSoldiers();

    loadSpawner();

    if (players.empty())
    {
        throw runtime_error("ERROR::loadLevel() at least 1 player is required");
    }
}

void LevelLoader::updateLevel()
{
    loadPhysicsObjects();
    loadWeapons();
}
        
void LevelLoader::getSpawner(Spawner*& spawner)
{
    spawner = this->spawner;
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
