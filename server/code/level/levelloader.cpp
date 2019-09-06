#include "../global/convert.hpp"

#include "../world/raytracer.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

#include "../physics_object/openglmotionstate.hpp"
#include "../physics_object/physicsobject.hpp"
#include "../physics_object/weapon.hpp"

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "levelloader.hpp"

LevelLoader::LevelLoader(World* physicsWorld)
{
    this->physicsWorld = physicsWorld;
}
        
void LevelLoader::loadPhysicsObject(XMLElement* physicsObjectElem, PhysicsObject*& PO)
{
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

void LevelLoader::loadWeapon(XMLElement* weaponElem, Weapon*& WE)
{
    const char* name = nullptr;
    weaponElem->QueryStringAttribute("name", &name);

    WE = new Weapon(name, physicsWorld->getWorld());

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

            shapeElem->QueryFloatAttribute("radius", &radius);
            shapeElem->QueryFloatAttribute("height", &height);
            shapeElem->QueryFloatAttribute("zup", &zup);

            WE->setShape(new btCylinderShape(btVector3(radius, height, zup)));    
            shape = true;
        }
        else if (!strcmp(type, "sphere"))
        {
            float radius = 0;

            shapeElem->QueryFloatAttribute("radius", &radius);

            WE->setShape(new btSphereShape(radius));    
            shape = true;
        }
        else if (!strcmp(type, "capsule"))
        {
            float radius = 0;
            float height = 0;

            shapeElem->QueryFloatAttribute("radius", &radius);
            shapeElem->QueryFloatAttribute("height", &height);

            WE->setShape(new btCapsuleShape(radius, height));    
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

                WE->setMass(mass);
            }

            /* position */
            XMLElement* positionElem = weaponElem->FirstChildElement("position");

            if (positionElem)
            {
                float x = 0, y = 0, z = 0;

                positionElem->QueryFloatAttribute("x", &x);
                positionElem->QueryFloatAttribute("y", &y);
                positionElem->QueryFloatAttribute("z", &z);

                WE->setPosition(btVector3(x, y, z));
            }

            /* rotation */
            XMLElement* rotationElem = weaponElem->FirstChildElement("rotation");

            if (rotationElem)
            {
                float x = 0, y = 0, z = 0, angle = 0;

                rotationElem->QueryFloatAttribute("x", &x);
                rotationElem->QueryFloatAttribute("y", &y);
                rotationElem->QueryFloatAttribute("z", &z);
                rotationElem->QueryFloatAttribute("angle", &angle);

                WE->setRotation(btQuaternion(btVector3(x, y, z), toRads(angle)));
            }

            /* angular factor */
            XMLElement* angularElem = weaponElem->FirstChildElement("angularfactor");

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

    XMLElement* weaponInfoElem = weaponElem->FirstChildElement("weaponinfo");

    /* info */
    if (weaponElem)
    {
        /* storage bullets */
        XMLElement* storageBulletsElem = weaponInfoElem->FirstChildElement("storagebullets");

        if (storageBulletsElem)
        {
            int amount = 0;

            storageBulletsElem->QueryIntAttribute("amount", &amount);

            WE->setStorageBullets(amount);
        }

        /* magazine size */
        XMLElement* magazineSizeElem = weaponInfoElem->FirstChildElement("magazinesize");

        if (magazineSizeElem)
        {
            int size = 0;

            magazineSizeElem->QueryIntAttribute("size", &size);

            WE->setMagazineSize(size);
        }

        /* magazine bullets */
        XMLElement* magazineBulletsElem = weaponInfoElem->FirstChildElement("magazinebullets");

        if (magazineBulletsElem)
        {
            int amount = 0;

            magazineBulletsElem->QueryIntAttribute("amount", &amount);

            WE->setMagazineBullets(amount);
        }

        /* shot speed */
        XMLElement* shotSpeedElem = weaponInfoElem->FirstChildElement("shotspeed");

        if (shotSpeedElem)
        {
            float speed = 0;

            shotSpeedElem->QueryFloatAttribute("speed", &speed);

            WE->setShotSpeed(speed);
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

void LevelLoader::loadWeapons()
{
    XMLDocument weaponDoc;

    weaponDoc.LoadFile((levelName + "/weapon.xml").c_str());

    XMLNode* root = weaponDoc.FirstChildElement("WeaponFile");

    if (!root)
    {
        throw runtime_error("ERROR::LevelLoader::loadWeapons() failed to load XML");
    }

    XMLNode* weaponsNode = root->FirstChildElement("weapons"); 
    XMLElement* weaponElem = weaponsNode->FirstChildElement("weapon");

    while (weaponElem)
    {
        Weapon* WE = nullptr;

        loadWeapon(weaponElem, WE);

        physicsObjects.insert({WE->getName(), WE});

        weaponElem = weaponElem->NextSiblingElement();
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

        /* physics object */
        XMLElement* physicsObjectElem = playerElem->FirstChildElement("physicsobject");

        if (physicsObjectElem)
        {
            PhysicsObject* PO = nullptr;

            loadPhysicsObject(physicsObjectElem, PO);

            physicsObjects.insert({PO->getName(), PO});

            player->setPhysicsObject(PO);
        }

        players.push_back(player);

        playerElem = playerElem->NextSiblingElement();
    }
}

void LevelLoader::loadSoldiers()
{
    XMLDocument soldierDoc;

    soldierDoc.LoadFile((levelName + "/soldier.xml").c_str());

    XMLNode* root = soldierDoc.FirstChildElement("SoldierFile");

    if (!root)
    {
        throw runtime_error("ERROR::LevelLoader::loadSoldiers() failed to load XML");
    }

    XMLNode* soldierNode = root->FirstChildElement("soldiers"); 
    XMLElement* soldierElem = soldierNode->FirstChildElement("soldier");

    while (soldierElem)
    {
        /* speed */
        XMLElement* speedElem = soldierElem->FirstChildElement("speed");
        float speed = 1.0;

        if (speedElem)
        {
            speedElem->QueryFloatAttribute("speed", &speed);
        }

        Soldier* soldier = new Soldier(speed);

        /* physics object */
        XMLElement* physicsObjectElem = soldierElem->FirstChildElement("physicsobject");

        if (physicsObjectElem)
        {
            PhysicsObject* PO = nullptr;

            loadPhysicsObject(physicsObjectElem, PO);

            physicsObjects.insert({PO->getName(), PO});

            soldier->setPhysicsObject(PO);
        }

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

        players.push_back(soldier);

        soldierElem = soldierElem->NextSiblingElement();
    }
}

void LevelLoader::loadLevel(string name)
{
    this->levelName = name;

    loadPhysicsObjects();
    loadWeapons();

    loadPlayers();
    loadSoldiers();

    if (players.empty())
    {
        throw runtime_error("ERROR::loadPlayers() at least 1 player is required");
    }
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
