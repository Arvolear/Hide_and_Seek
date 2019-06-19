#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../debug/debugsphere.hpp"
#include "../debug/debugdrawer.hpp"

#include "../player/camera.hpp"

#include "../world/raytracer.hpp"
#include "../world/constrainthandler.hpp"
#include "../world/bulletevents.hpp"
#include "../world/world.hpp"

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

#include "../player/player.hpp"

#include "dirlight.hpp"
#include "skybox.hpp"
#include "levelloader.hpp"

LevelLoader::LevelLoader(Window* window, World* physicsWorld)
{
    this->window = window;
    this->physicsWorld = physicsWorld;

    skyBox = nullptr;
    player = nullptr;

    projection = mat4(1.0);
}

void LevelLoader::loadObjects()
{
    XMLDocument gameObjectDoc;
    
    gameObjectDoc.LoadFile((levelName + "/game_object.xml").c_str());
    
    XMLNode* root = gameObjectDoc.FirstChildElement("GameObjectFile");

    if (!root)
    {
        throw runtime_error("ERROR::loadObjects() failed to load XML");
    }
    
    XMLNode* gameObjectsNode = root->FirstChildElement("gameobjects"); 
    XMLElement* gameObjectElem = gameObjectsNode->FirstChildElement("gameobject");

    while (gameObjectElem)
    {
        const char* name = nullptr;
        gameObjectElem->QueryStringAttribute("name", &name);

        GameObject* GO = new GameObject(name);

        /* graphics object */
        XMLElement* graphicsObjectElem = gameObjectElem->FirstChildElement("graphicsobject");

        if (graphicsObjectElem)
        {
            const char* path = nullptr;
            graphicsObjectElem->QueryStringAttribute("path", &path);

            GO->setGraphicsObject(levelName + path);
        }

        /* physics object */
        XMLElement* physicsObjectElem = gameObjectElem->FirstChildElement("physicsobject");

        if (physicsObjectElem)
        {
            GO->setPhysicsObject(new PhysicsObject(physicsWorld->getWorld()));

            /* shape */
            XMLElement* shapeElem = physicsObjectElem->FirstChildElement("shape");

            const char* type = nullptr;
            shapeElem->QueryStringAttribute("type", &type);
 
            if (!strcmp(type, "box"))
            {
                float x = 0, y = 0, z = 0;

                shapeElem->QueryFloatAttribute("x", &x);
                shapeElem->QueryFloatAttribute("y", &y);
                shapeElem->QueryFloatAttribute("z", &z);

                GO->getPhysicsObject()->setShape(new btBoxShape(btVector3(x, y, z)));    
            }
            else if (!strcmp(type, "cylinder"))
            {
                float radius = 0, height = 0, zup = 0;

                shapeElem->QueryFloatAttribute("radius", &radius);
                shapeElem->QueryFloatAttribute("height", &height);
                shapeElem->QueryFloatAttribute("zup", &zup);

                GO->getPhysicsObject()->setShape(new btCylinderShape(btVector3(radius, height, zup)));    
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

                    childShapeElem = childShapeElem->NextSiblingElement();
                }

                GO->getPhysicsObject()->setShape(CS);    
            }

            /* mass */
            XMLElement* massElem = physicsObjectElem->FirstChildElement("mass");

            if (massElem)
            {
                float mass = 0;
                massElem->QueryFloatAttribute("mass", &mass);

                GO->getPhysicsObject()->setMass(mass);
            }

            /* position */
            XMLElement* positionElem = physicsObjectElem->FirstChildElement("position");

            if (positionElem)
            {
                float x = 0, y = 0, z = 0;

                positionElem->QueryFloatAttribute("x", &x);
                positionElem->QueryFloatAttribute("y", &y);
                positionElem->QueryFloatAttribute("z", &z);

                GO->getPhysicsObject()->setPosition(btVector3(x, y, z));
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

                GO->getPhysicsObject()->setRotation(btQuaternion(btVector3(x, y, z), toRads(angle)));
            }
        }

        gameObjects.insert({GO->getName(), GO});

        gameObjectElem = gameObjectElem->NextSiblingElement();
    }
}

void LevelLoader::loadDirLight()
{
    XMLDocument dirLightDoc;

    dirLightDoc.LoadFile((levelName + "/dirlight.xml").c_str());

    XMLNode* root = dirLightDoc.FirstChildElement("DirLightFile");

    if (!root)
    {
        throw runtime_error("ERROR::loadDirLight() failed to load XML");
    }

    XMLNode* dirLightsNode = root->FirstChildElement("dirlights"); 
    XMLElement* dirLightElem = dirLightsNode->FirstChildElement("dirlight");

    while (dirLightElem)
    {
        DirLight* DL = new DirLight();

        XMLElement* direction = dirLightElem->FirstChildElement("direction");

        if (direction)
        {
            float x = 0, y = 0, z = 0;
            direction->QueryFloatAttribute("x", &x);
            direction->QueryFloatAttribute("y", &y);
            direction->QueryFloatAttribute("z", &z);

            DL->setDirection(vec3(x, y, z));
        }

        XMLElement* ambient = dirLightElem->FirstChildElement("ambient");

        if (ambient)
        {
            float r = 0, g = 0, b = 0;
            ambient->QueryFloatAttribute("r", &r);
            ambient->QueryFloatAttribute("g", &g);
            ambient->QueryFloatAttribute("b", &b);

            DL->setAmbient(vec3(r, g, b));
        }

        XMLElement* diffuse = dirLightElem->FirstChildElement("diffuse");

        if (diffuse)
        {
            float r = 0, g = 0, b = 0;
            diffuse->QueryFloatAttribute("r", &r);
            diffuse->QueryFloatAttribute("g", &g);
            diffuse->QueryFloatAttribute("b", &b);

            DL->setDiffuse(vec3(r, g, b));
        }

        XMLElement* specular = dirLightElem->FirstChildElement("specular");

        if (specular)
        {
            float r = 0, g = 0, b = 0;
            specular->QueryFloatAttribute("r", &r);
            specular->QueryFloatAttribute("g", &g);
            specular->QueryFloatAttribute("b", &b);

            DL->setSpecular(vec3(r, g, b));
        }

        XMLElement* shadowBuffer = dirLightElem->FirstChildElement("shadowbuffer");

        if (shadowBuffer)
        {
            float x = 0, y = 0;
            shadowBuffer->QueryFloatAttribute("x", &x);
            shadowBuffer->QueryFloatAttribute("y", &y);

            DL->genShadowBuffer(x, y);
        }

        XMLElement* proj = dirLightElem->FirstChildElement("projection");

        const char* type = nullptr;
        proj->QueryStringAttribute("type", &type);
        mat4 shadowProj = mat4(1.0);

        if (!strcmp(type, "perspective"))
        {
            float fovy = 0, aspect = 0, near = 0, far = 0;

            proj->QueryFloatAttribute("fovy", &fovy);
            proj->QueryFloatAttribute("aspect", &aspect);
            proj->QueryFloatAttribute("near", &near);
            proj->QueryFloatAttribute("far", &far);

            shadowProj = perspective(fovy, aspect, near, far);
        }
        else if (!strcmp(type, "orthogonal"))
        {
            float left = 0, right = 0, bottom = 0, top = 0, near = 0, far = 0;

            proj->QueryFloatAttribute("left", &left);
            proj->QueryFloatAttribute("right", &right);
            proj->QueryFloatAttribute("bottom", &bottom);
            proj->QueryFloatAttribute("top", &top);
            proj->QueryFloatAttribute("near", &near);
            proj->QueryFloatAttribute("far", &far);

            shadowProj = ortho(left, right, bottom, top, near, far); 
        }

        DL->setProjection(shadowProj);

        if (find(dirLights.begin(), dirLights.end(), DL) == dirLights.end())
        {
            dirLights.push_back(DL); 
        }

        dirLightElem = dirLightElem->NextSiblingElement();
    }
}

void LevelLoader::loadSkyBox()
{
    XMLDocument skyBoxDoc;

    skyBoxDoc.LoadFile((levelName + "/skybox.xml").c_str());

    XMLNode* root = skyBoxDoc.FirstChildElement("SkyBoxFile");

    if (!root)
    {
        throw runtime_error("ERROR::loadSkyBox() failed to load XML");
    }

    XMLNode* skyboxes = root->FirstChildElement("skyboxes"); 
    XMLElement* skybox = skyboxes->FirstChildElement("skybox");

    const char* path = nullptr;
    skybox->QueryStringAttribute("path", &path);

    skyBox = new SkyBox();
    skyBox->init();

    skyBox->loadSkyBox(levelName + path);
}

void LevelLoader::loadPlayer()
{
    /* TODO PARSE XML */
    GameObject* P = new GameObject("player");
    P->setPhysicsObject(new PhysicsObject(physicsWorld->getWorld()));

    CompoundShape* CS = new CompoundShape;

    CS->add(new btSphereShape(0.4), btVector3(0, 1.75, 0), btQuaternion(btVector3(0, 0, 1), 0));
    CS->add(new btCapsuleShape(0.4, 3.5), btVector3(0, 0, 0), btQuaternion(btVector3(0, 0, 1), 0));

    P->getPhysicsObject()->setShape(CS);
    P->getPhysicsObject()->setPosition(btVector3(10, 3, 10));
    P->getPhysicsObject()->setMass(10);
    //P->getPhysicsObject()->setRotation(btQuaternion(btVector3(1, 0, 0), toRads(30)));
    P->getPhysicsObject()->getRigidBody()->setAngularFactor(btVector3(0, 0, 0));

    RayTracer* rayTracer = new RayTracer(physicsWorld->getWorld(), nullptr, projection);     

    player = new Player(window, vec3(0, 5, 5), vec3(0, 0, -1));
    player->setRayTracer(rayTracer);
    //player->setGameObject(P);
    //player->setOffset(vec2(0.43, 0));
    rayTracer->setCamera(player);
}

void LevelLoader::loadProjection()
{
    XMLDocument projDoc;
    projDoc.LoadFile((levelName + "/projection.xml").c_str());

    XMLNode* root = projDoc.FirstChildElement("ProjectionFile");

    if (!root)
    {
        throw runtime_error("ERROR::loadProjection() failed to load XML");
    }

    XMLNode* projections = root->FirstChildElement("projections");
    XMLElement* proj = projections->FirstChildElement("projection");

    const char* type = nullptr;
    proj->QueryStringAttribute("type", &type);

    if (!strcmp(type, "perspective"))
    {
        float fovy = 0, aspect = 0, near = 0, far = 0;

        proj->QueryFloatAttribute("fovy", &fovy);
        proj->QueryFloatAttribute("aspect", &aspect);
        proj->QueryFloatAttribute("near", &near);
        proj->QueryFloatAttribute("far", &far);

        projection = perspective(fovy, aspect, near, far);
    }
    else if (!strcmp(type, "orthogonal"))
    {
        float left = 0, right = 0, bottom = 0, top = 0, near = 0, far = 0;

        proj->QueryFloatAttribute("left", &left);
        proj->QueryFloatAttribute("right", &right);
        proj->QueryFloatAttribute("bottom", &bottom);
        proj->QueryFloatAttribute("top", &top);
        proj->QueryFloatAttribute("near", &near);
        proj->QueryFloatAttribute("far", &far);

        projection = ortho(left, right, bottom, top, near, far); 
    }
}

void LevelLoader::loadLevel(string name)
{
    this->levelName = name;

    loadObjects();
    loadDirLight();
    loadSkyBox();

    loadProjection();

    loadPlayer();
}

void LevelLoader::getGameObjectsData(map < string, GameObject* > &gameObjects) const
{
    gameObjects = this->gameObjects;
}

void LevelLoader::getDirLightData(vector < DirLight* > &dirLights) const
{
    dirLights = this->dirLights;
}

void LevelLoader::getSkyBoxData(SkyBox*& skyBox) const
{
    skyBox = this->skyBox;
}

void LevelLoader::getProjectionData(mat4 &projection) const
{
    projection = this->projection;
}

void LevelLoader::getPlayerData(Player*& player) const
{
    player = this->player;
}

LevelLoader::~LevelLoader() {}
