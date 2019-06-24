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

    projection = mat4(1.0);
}
        
mat4 LevelLoader::loadProjection(XMLElement* projElem)
{
    mat4 proj = mat4(1.0);

    const char* type = nullptr;
    projElem->QueryStringAttribute("type", &type);

    if (!strcmp(type, "perspective"))
    {
        float fovy = 0, aspect = 0, near = 0, far = 0;

        projElem->QueryFloatAttribute("fovy", &fovy);
        projElem->QueryFloatAttribute("aspect", &aspect);
        projElem->QueryFloatAttribute("near", &near);
        projElem->QueryFloatAttribute("far", &far);

        proj = perspective(fovy, aspect, near, far);
    }
    else if (!strcmp(type, "orthogonal"))
    {
        float left = 0, right = 0, bottom = 0, top = 0, near = 0, far = 0;

        projElem->QueryFloatAttribute("left", &left);
        projElem->QueryFloatAttribute("right", &right);
        projElem->QueryFloatAttribute("bottom", &bottom);
        projElem->QueryFloatAttribute("top", &top);
        projElem->QueryFloatAttribute("near", &near);
        projElem->QueryFloatAttribute("far", &far);

        proj = ortho(left, right, bottom, top, near, far);
    }

    return proj;
}

GameObject* LevelLoader::loadGameObject(XMLElement* gameObjectElem)
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


        /* view frustum */
        XMLElement* viewFrustumElem = graphicsObjectElem->FirstChildElement("viewfrustum");

        if (viewFrustumElem)
        {
            const char* apply = nullptr;
            viewFrustumElem->QueryStringAttribute("apply", &apply);

            if (!strcmp(apply, "true"))
            {
                GO->setViewFrustum(viewFrustum);
            }
        }

        /* scale */
        XMLElement* scaleElem = graphicsObjectElem->FirstChildElement("scale");

        if (scaleElem)
        {
            float x = 0, y = 0, z = 0;

            scaleElem->QueryFloatAttribute("x", &x);
            scaleElem->QueryFloatAttribute("y", &y);
            scaleElem->QueryFloatAttribute("z", &z);

            GO->setLocalScale(vec3(x, y, z));
        }

        /* position */
        XMLElement* positionElem = graphicsObjectElem->FirstChildElement("position");

        if (positionElem)
        {
            float x = 0, y = 0, z = 0;

            positionElem->QueryFloatAttribute("x", &x);
            positionElem->QueryFloatAttribute("y", &y);
            positionElem->QueryFloatAttribute("z", &z);

            GO->setLocalPosition(vec3(x, y, z));
        }

        /* rotation */
        XMLElement* rotationElem = graphicsObjectElem->FirstChildElement("rotation");

        if (rotationElem)
        {
            float x = 0, y = 0, z = 0, angle = 0;

            rotationElem->QueryFloatAttribute("x", &x);
            rotationElem->QueryFloatAttribute("y", &y);
            rotationElem->QueryFloatAttribute("z", &z);
            rotationElem->QueryFloatAttribute("angle", &angle);

            GO->setLocalRotation(vec3(x, y, z), angle);
        }

        /* animations */
        XMLNode* animationsNode = graphicsObjectElem->FirstChildElement("animations");

        if (animationsNode)
        {
            XMLElement* animationElem = animationsNode->FirstChildElement("animation");

            while (animationElem)
            {
                const char* name = nullptr;
                animationElem->QueryStringAttribute("name", &name);

                Animation* anim = new Animation(name);

                /* anim id */
                XMLElement* animIdElem = animationElem->FirstChildElement("animid");

                if (animIdElem)
                {
                    int id = 0;

                    animIdElem->QueryIntAttribute("id", &id);

                    anim->setAnimId(id);
                }

                /* frames range */
                XMLElement* framesRangeElem = animationElem->FirstChildElement("framesrange");

                if (framesRangeElem)
                {
                    float start = 0, end = 0;

                    framesRangeElem->QueryFloatAttribute("start", &start);
                    framesRangeElem->QueryFloatAttribute("end", &end);

                    anim->setFramesRange(vec2(start, end));
                }

                /* speed */
                XMLElement* speedElem = animationElem->FirstChildElement("speed");

                if (speedElem)
                {
                    float speed = 0;

                    speedElem->QueryFloatAttribute("speed", &speed);

                    anim->setSpeed(speed);
                }

                /* loop */
                XMLElement* loopElem = animationElem->FirstChildElement("loop");

                if (loopElem)
                {
                    const char* loop = nullptr;

                    loopElem->QueryStringAttribute("loop", &loop);

                    if (!strcmp("true", loop))
                    {
                        anim->setLoop(true);
                    }
                }

                GO->addAnimation(anim);

                /* active */
                const char* active = nullptr;
                animationElem->QueryStringAttribute("active", &active);

                if (!strcmp("true", active))
                {
                    GO->playAnimation(anim->getName(), true); 
                }

                animationElem = animationElem->NextSiblingElement();
            }
        }
    }

    /* physics object */
    XMLElement* physicsObjectElem = gameObjectElem->FirstChildElement("physicsobject");

    if (physicsObjectElem)
    {
        GO->setPhysicsObject(new PhysicsObject(physicsWorld->getWorld()));

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

                GO->getPhysicsObject()->setShape(new btBoxShape(btVector3(x, y, z)));    
                shape = true;
            }
            else if (!strcmp(type, "cylinder"))
            {
                float radius = 0, height = 0, zup = 0;

                shapeElem->QueryFloatAttribute("radius", &radius);
                shapeElem->QueryFloatAttribute("height", &height);
                shapeElem->QueryFloatAttribute("zup", &zup);

                GO->getPhysicsObject()->setShape(new btCylinderShape(btVector3(radius, height, zup)));    
                shape = true;
            }
            else if (!strcmp(type, "sphere"))
            {
                float radius = 0;

                shapeElem->QueryFloatAttribute("radius", &radius);

                GO->getPhysicsObject()->setShape(new btSphereShape(radius));    
                shape = true;
            }
            else if (!strcmp(type, "capsule"))
            {
                float radius = 0;
                float height = 0;

                shapeElem->QueryFloatAttribute("radius", &radius);
                shapeElem->QueryFloatAttribute("height", &height);

                GO->getPhysicsObject()->setShape(new btCapsuleShape(radius, height));    
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

                GO->getPhysicsObject()->setShape(CS);    
            }

            if (shape)
            {
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

                /* angular factor */
                XMLElement* angularElem = physicsObjectElem->FirstChildElement("angularfactor");

                if (angularElem)
                {
                    float x = 0, y = 0, z = 0;

                    angularElem->QueryFloatAttribute("x", &x);
                    angularElem->QueryFloatAttribute("y", &y);
                    angularElem->QueryFloatAttribute("z", &z);

                    GO->getPhysicsObject()->getRigidBody()->setAngularFactor(btVector3(x, y, z));
                }
            }
        }
    }

    /* debug object */
    XMLElement* debugObjectElem = gameObjectElem->FirstChildElement("debugobject");

    if (debugObjectElem)
    {
        XMLElement* debugSphereElem = debugObjectElem->FirstChildElement("debugsphere");

        /* debug sphere */
        if (debugSphereElem)
        {
            int detail = 0;

            debugSphereElem->QueryIntAttribute("detail", &detail);

            GO->createDebugSphere(detail);
        }
    }

    return GO;
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
        GameObject* GO = loadGameObject(gameObjectElem);

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

        XMLElement* directionElem = dirLightElem->FirstChildElement("direction");

        if (directionElem)
        {
            float x = 0, y = 0, z = 0;
            directionElem->QueryFloatAttribute("x", &x);
            directionElem->QueryFloatAttribute("y", &y);
            directionElem->QueryFloatAttribute("z", &z);

            DL->setDirection(vec3(x, y, z));
        }

        XMLElement* ambientElem = dirLightElem->FirstChildElement("ambient");

        if (ambientElem)
        {
            float r = 0, g = 0, b = 0;
            ambientElem->QueryFloatAttribute("r", &r);
            ambientElem->QueryFloatAttribute("g", &g);
            ambientElem->QueryFloatAttribute("b", &b);

            DL->setAmbient(vec3(r, g, b));
        }

        XMLElement* diffuseElem = dirLightElem->FirstChildElement("diffuse");

        if (diffuseElem)
        {
            float r = 0, g = 0, b = 0;
            diffuseElem->QueryFloatAttribute("r", &r);
            diffuseElem->QueryFloatAttribute("g", &g);
            diffuseElem->QueryFloatAttribute("b", &b);

            DL->setDiffuse(vec3(r, g, b));
        }

        XMLElement* specularElem = dirLightElem->FirstChildElement("specular");

        if (specularElem)
        {
            float r = 0, g = 0, b = 0;
            specularElem->QueryFloatAttribute("r", &r);
            specularElem->QueryFloatAttribute("g", &g);
            specularElem->QueryFloatAttribute("b", &b);

            DL->setSpecular(vec3(r, g, b));
        }

        XMLElement* shadowBufferElem = dirLightElem->FirstChildElement("shadowbuffer");

        if (shadowBufferElem)
        {
            float x = 0, y = 0;
            shadowBufferElem->QueryFloatAttribute("x", &x);
            shadowBufferElem->QueryFloatAttribute("y", &y);

            DL->genShadowBuffer(x, y);
        }

        XMLElement* projElem = dirLightElem->FirstChildElement("projection");

        mat4 shadowProj = loadProjection(projElem);

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

    XMLNode* skyboxesNode = root->FirstChildElement("skyboxes"); 
    XMLElement* skyboxElem = skyboxesNode->FirstChildElement("skybox");

    const char* path = nullptr;
    skyboxElem->QueryStringAttribute("path", &path);

    skyBox = new SkyBox();
    skyBox->init();

    skyBox->loadSkyBox(levelName + path);
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
        /* position */
        XMLElement* positionElem = playerElem->FirstChildElement("position");
        vec3 position(0.0);

        if (positionElem)
        {
            float x = 0, y = 0, z = 0;
            positionElem->QueryFloatAttribute("x", &x);
            positionElem->QueryFloatAttribute("y", &y);
            positionElem->QueryFloatAttribute("z", &z);

            position = vec3(x, y, z);
        }

        /* forward */
        XMLElement* forwardElem = playerElem->FirstChildElement("forward");
        vec3 forward(0.0);

        if (forwardElem)
        {
            float x = 0, y = 0, z = 0;
            forwardElem->QueryFloatAttribute("x", &x);
            forwardElem->QueryFloatAttribute("y", &y);
            forwardElem->QueryFloatAttribute("z", &z);

            forward = vec3(x, y, z);
        }

        /* speed */
        XMLElement* speedElem = playerElem->FirstChildElement("speed");
        float speed = 1.0;

        if (speedElem)
        {
            speedElem->QueryFloatAttribute("speed", &speed);
        }

        Player* player = new Player(window, position, forward, speed);

        /* game object */
        XMLElement* gameObjectElem = playerElem->FirstChildElement("gameobject");

        if (gameObjectElem)
        {
            GameObject* GO = loadGameObject(gameObjectElem);

            gameObjects.insert({GO->getName(), GO});

            player->setGameObject(GO);
        }

        /* raytracer */
        XMLElement* rayTracerElem = playerElem->FirstChildElement("raytracer");

        if (rayTracerElem)
        {
            const char* apply = nullptr;

            rayTracerElem->QueryStringAttribute("apply", &apply);

            if (!strcmp("true", apply))
            {
                RayTracer* rayTracer = new RayTracer(physicsWorld->getWorld(), nullptr, projection);     

                player->setRayTracer(rayTracer);
                rayTracer->setCamera(player);
            }
        }

        /* offset */
        XMLElement* offsetElem = playerElem->FirstChildElement("offset");

        if (offsetElem)
        {
            float x = 0, y = 0;

            offsetElem->QueryFloatAttribute("x", &x);
            offsetElem->QueryFloatAttribute("y", &y);

            player->setOffset(vec2(x, y));
        }

        players.push_back(player);

        playerElem = playerElem->NextSiblingElement();
    }

    if (players.empty())
    {
        throw runtime_error("ERROR::loadPlayers() at least 1 player is required");
    }
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

    XMLNode* projectionsNode = root->FirstChildElement("projections");
    XMLElement* projElem = projectionsNode->FirstChildElement("projection");

    projection = loadProjection(projElem);

    viewFrustum = new ViewFrustum;
}

void LevelLoader::loadLevel(string name)
{
    this->levelName = name;

    loadProjection();

    loadPlayers();

    loadSkyBox();
    loadDirLight();
    loadObjects();
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

void LevelLoader::getViewFrustumData(ViewFrustum*& viewFrustum)
{
    viewFrustum = this->viewFrustum;
}

void LevelLoader::getPlayersData(vector < Player* > &players) const
{
    players = this->players;
}

LevelLoader::~LevelLoader() {}
