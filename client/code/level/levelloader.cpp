#include "../global/globaluse.hpp"

#include "../shader/shader.hpp"

#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/colorbuffer.hpp"
#include "../framebuffer/depthbuffer.hpp"
#include "../framebuffer/shadowbuffer.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "../global/radialblur.hpp"
#include "../global/gaussianblur.hpp"

#include "../debug/debugdrawer.hpp"

#include "../player/camera.hpp"

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

#include "../player/player.hpp"
#include "../player/soldier.hpp"

#include "ssao.hpp"
#include "atmosphere.hpp"
#include "dirlightsoftshadow.hpp"
#include "dirlight.hpp"
#include "skybox.hpp"
#include "levelloader.hpp"

LevelLoader::LevelLoader(Window* window, World* physicsWorld)
{
    this->window = window;
    this->physicsWorld = physicsWorld;

    sSAO = nullptr;
    atmosphere = nullptr;
    skyBox = nullptr;

    projection = mat4(1.0);
}
        
void LevelLoader::loadProjection(XMLElement* projElem, mat4 &proj)
{
    if (projElem)
    {
        proj = mat4(1.0);

        const char* type = nullptr;
        projElem->QueryStringAttribute("type", &type);

        if (!strcmp(type, "perspective"))
        {
            float fovy = 0, aspect = 0, near = 0, far = 0;

            projElem->QueryFloatAttribute("fovy", &fovy);
            projElem->QueryFloatAttribute("aspect", &aspect);
            projElem->QueryFloatAttribute("near", &near);
            projElem->QueryFloatAttribute("far", &far);

            proj = perspective(radians(fovy), aspect, near, far);
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
    }
}

void LevelLoader::loadAnimation(XMLElement* animationElem, Animation*& anim)
{
    const char* name = nullptr;
    animationElem->QueryStringAttribute("name", &name);

    anim = new Animation(name);

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
        bool loop = false;

        loopElem->QueryBoolAttribute("loop", &loop);

        anim->setLoop(loop);
    }
}

void LevelLoader::loadGraphicsObject(XMLElement* graphicsObjectElem, GameObject*& GO)
{
    const char* path = nullptr;
    graphicsObjectElem->QueryStringAttribute("path", &path);

    GO->setGraphicsObject(levelName + path);


    /* view frustum */
    XMLElement* viewFrustumElem = graphicsObjectElem->FirstChildElement("viewfrustum");

    if (viewFrustumElem)
    {
        bool apply = false;
        viewFrustumElem->QueryBoolAttribute("apply", &apply);

        if (apply)
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
            Animation* anim = nullptr;
            loadAnimation(animationElem, anim);

            GO->addAnimation(anim);

            /* active */
            bool active = false;
            animationElem->QueryBoolAttribute("active", &active);

            if (active)
            {
                GO->playAnimation(anim->getName(), true); 
            }

            animationElem = animationElem->NextSiblingElement();
        }
    }
}

void LevelLoader::loadPhysicsObject(XMLElement* physicsObjectElem, GameObject*& GO)
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

                    childRotation = btQuaternion(btVector3(x, y, z), global.toRads(angle));
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

                GO->getPhysicsObject()->setRotation(btQuaternion(btVector3(x, y, z), global.toRads(angle)));
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

void LevelLoader::loadDebugObject(XMLElement* debugObjectElem, GameObject*& GO)
{
    XMLElement* debugSphereElem = debugObjectElem->FirstChildElement("debugsphere");

    /* debug sphere */
    if (debugSphereElem)
    {
        int detail = 1;

        debugSphereElem->QueryIntAttribute("detail", &detail);

        GO->createDebugSphere(detail);
    }
}

void LevelLoader::loadGameObject(XMLElement* gameObjectElem, GameObject*& GO)
{
    const char* name = nullptr;
    gameObjectElem->QueryStringAttribute("name", &name);

    GO = new GameObject(name);

    /* graphics object */
    XMLElement* graphicsObjectElem = gameObjectElem->FirstChildElement("graphicsobject");

    if (graphicsObjectElem)
    {
        loadGraphicsObject(graphicsObjectElem, GO);
    }

    /* physics object */
    XMLElement* physicsObjectElem = gameObjectElem->FirstChildElement("physicsobject");

    if (physicsObjectElem)
    {
        loadPhysicsObject(physicsObjectElem, GO);
    }

    /* debug object */
    XMLElement* debugObjectElem = gameObjectElem->FirstChildElement("debugobject");

    if (debugObjectElem)
    {
        loadDebugObject(debugObjectElem, GO);
    }
}

void LevelLoader::loadRifle(XMLElement* rifleElem, Rifle*& rifle)
{
    const char* name = nullptr;
    rifleElem->QueryStringAttribute("name", &name);

    GameObject* GO = new Rifle(window, name);

    /* graphics object */
    XMLElement* graphicsObjectElem = rifleElem->FirstChildElement("graphicsobject");

    if (graphicsObjectElem)
    {
        loadGraphicsObject(graphicsObjectElem, GO);
    }

    /* physics object */
    XMLElement* physicsObjectElem = rifleElem->FirstChildElement("physicsobject");

    if (physicsObjectElem)
    {
        loadPhysicsObject(physicsObjectElem, GO);
    }

    /* debug object */
    XMLElement* debugObjectElem = rifleElem->FirstChildElement("debugobject");

    if (debugObjectElem)
    {
        loadDebugObject(debugObjectElem, GO);
    }

    rifle = dynamic_cast < Rifle* >(GO);

    /* info */
    XMLElement* rifleInfoElem = rifleElem->FirstChildElement("rifleinfo");

    if (rifleInfoElem)
    {
        /* offset */
        XMLElement* offsetElem = rifleInfoElem->FirstChildElement("offset");

        if (offsetElem)
        {
            float x = 0, y = 0, z = 0;

            offsetElem->QueryFloatAttribute("x", &x);
            offsetElem->QueryFloatAttribute("y", &y);
            offsetElem->QueryFloatAttribute("z", &z);

            rifle->setOffset(vec3(x, y, z));
        }

        /* twist */
        XMLElement* twistElem = rifleInfoElem->FirstChildElement("twist");

        if (twistElem)
        {
            float x = 0, y = 0, z = 0, angle = 0;

            twistElem->QueryFloatAttribute("x", &x);
            twistElem->QueryFloatAttribute("y", &y);
            twistElem->QueryFloatAttribute("z", &z);
            twistElem->QueryFloatAttribute("angle", &angle);

            rifle->setTwist(vec3(x, y, z), global.toRads(angle));
        }

        /* storage bullets */
        XMLElement* storageBulletsElem = rifleInfoElem->FirstChildElement("storagebullets");

        if (storageBulletsElem)
        {
            int amount = 0;

            storageBulletsElem->QueryIntAttribute("amount", &amount);

            rifle->setStorageBullets(amount);
        }

        /* magazine size */
        XMLElement* magazineSizeElem = rifleInfoElem->FirstChildElement("magazinesize");

        if (magazineSizeElem)
        {
            int size = 0;

            magazineSizeElem->QueryIntAttribute("size", &size);

            rifle->setMagazineSize(size);
        }

        /* magazine bullets */
        XMLElement* magazineBulletsElem = rifleInfoElem->FirstChildElement("magazinebullets");

        if (magazineBulletsElem)
        {
            int amount = 0;

            magazineBulletsElem->QueryIntAttribute("amount", &amount);

            rifle->setMagazineBullets(amount);
        }

        /* shot speed */
        XMLElement* shotSpeedElem = rifleInfoElem->FirstChildElement("shotspeed");

        if (shotSpeedElem)
        {
            float speed = 0;

            shotSpeedElem->QueryFloatAttribute("speed", &speed);

            rifle->setShotSpeed(speed);
        }
    }
}

void LevelLoader::loadGameObjects()
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
        GameObject* GO = nullptr;
        loadGameObject(gameObjectElem, GO);

        gameObjects.insert({GO->getName(), GO});

        gameObjectElem = gameObjectElem->NextSiblingElement();
    }
}

void LevelLoader::loadRifles()
{
    XMLDocument rifleDoc;

    rifleDoc.LoadFile((levelName + "/rifle.xml").c_str());

    XMLNode* root = rifleDoc.FirstChildElement("RifleFile");

    if (!root)
    {
        throw runtime_error("ERROR::loadRifles() failed to load XML"); 
    }

    XMLNode* riflesNode = root->FirstChildElement("rifles");
    XMLElement* rifleElem = riflesNode->FirstChildElement("rifle");

    while (rifleElem)
    {
        Rifle* rifle = nullptr;
        loadRifle(rifleElem, rifle);

        gameObjects.insert({rifle->getName(), rifle});

        rifleElem = rifleElem->NextSiblingElement();
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

        XMLElement* colorElem = dirLightElem->FirstChildElement("color");

        if (colorElem)
        {
            float r = 0, g = 0, b = 0;
            colorElem->QueryFloatAttribute("r", &r);
            colorElem->QueryFloatAttribute("g", &g);
            colorElem->QueryFloatAttribute("b", &b);

            DL->setColor(vec3(r, g, b));
        }

        /* scatter */
        XMLElement* scatterElem = dirLightElem->FirstChildElement("scatter");

        if (scatterElem)
        {
            /* sphere */
            XMLElement* sphereElem = scatterElem->FirstChildElement("sphere");

            if (sphereElem)
            {
                XMLElement* colorElem = sphereElem->FirstChildElement("color");

                if (colorElem)
                {
                    float r = 0, g = 0, b = 0;
                    colorElem->QueryFloatAttribute("r", &r);
                    colorElem->QueryFloatAttribute("g", &g);
                    colorElem->QueryFloatAttribute("b", &b);

                    DL->setSphereColor(vec3(r, g, b));
                }

                XMLElement* centerElem = sphereElem->FirstChildElement("center");

                vec3 center(0);

                if (centerElem)
                {
                    centerElem->QueryFloatAttribute("x", &center.x);
                    centerElem->QueryFloatAttribute("y", &center.y);
                    centerElem->QueryFloatAttribute("z", &center.z);
                }

                XMLElement* radiusElem = sphereElem->FirstChildElement("radius");

                float radius = 0;

                if (radiusElem)
                {
                    radiusElem->QueryFloatAttribute("radius", &radius);
                }

                XMLElement* qualityElem = sphereElem->FirstChildElement("quality");

                int quality = 0;

                if (qualityElem)
                {
                    qualityElem->QueryIntAttribute("quality", &quality);
                }

                DL->genSphere(center, radius, quality);
            }

            XMLElement* blurScaleElem = scatterElem->FirstChildElement("blurscale");

            float scale = 1.0;

            if (blurScaleElem)
            {
                blurScaleElem->QueryFloatAttribute("scale", &scale);
            }

            DL->genScatterBuffer(window->getRenderSize(), scale);

            XMLElement* exposureElem = scatterElem->FirstChildElement("exposure");

            float exposure = 1.0;

            if (exposureElem)
            {
                exposureElem->QueryFloatAttribute("exposure", &exposure);
            }

            DL->setRadialExposure(exposure);

            XMLElement* decayElem = scatterElem->FirstChildElement("decay");

            float decay = 1.0;

            if (decayElem)
            {
                decayElem->QueryFloatAttribute("decay", &decay);
            }

            DL->setRadialDecay(decay);

            XMLElement* densityElem = scatterElem->FirstChildElement("density");

            float density = 1.0;

            if (densityElem)
            {
                densityElem->QueryFloatAttribute("density", &density);
            }

            DL->setRadialDensity(density);

            XMLElement* weightElem = scatterElem->FirstChildElement("weight");

            float weight = 1.0;

            if (weightElem)
            {
                weightElem->QueryFloatAttribute("weight", &weight);
            }

            DL->setRadialWeight(weight);
        }

        /* shadow */
        XMLElement* shadowElem = dirLightElem->FirstChildElement("shadow");

        if (shadowElem)
        {
            XMLElement* shadowBufferElem = shadowElem->FirstChildElement("shadowbuffer");

            float x = 500, y = 500;

            if (shadowBufferElem)
            {
                shadowBufferElem->QueryFloatAttribute("x", &x);
                shadowBufferElem->QueryFloatAttribute("y", &y);
            }

            DL->genShadowBuffer(x, y);
            
            XMLElement* intensityElem = shadowElem->FirstChildElement("intensity");
                
            float intensity = 0.0;

            if (intensityElem)
            {
                intensityElem->QueryFloatAttribute("intensity", &intensity);
            }
           
            DL->setShadowIntensity(intensity);
            
            XMLElement* biasElem = shadowElem->FirstChildElement("bias");
                
            float bias = 0.0;

            if (biasElem)
            {
                biasElem->QueryFloatAttribute("bias", &bias);
            }
           
            DL->setShadowBias(bias);
            
            XMLElement* blurScaleElem = shadowElem->FirstChildElement("blurscale");

            float scale = 1.0;

            if (blurScaleElem)
            {
                blurScaleElem->QueryFloatAttribute("scale", &scale);
            }
            
            DL->setShadowSoftness(scale);

            XMLElement* projElem = shadowElem->FirstChildElement("projection");

            mat4 shadowProj = mat4(1.0);
            loadProjection(projElem, shadowProj);

            DL->setShadowProjection(shadowProj);
        }

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

void LevelLoader::loadAtmosphere()
{
    XMLDocument atmosphereDoc;

    atmosphereDoc.LoadFile((levelName + "/atmosphere.xml").c_str());

    XMLNode* root = atmosphereDoc.FirstChildElement("AtmosphereFile");

    if (!root)
    {
        throw runtime_error("ERROR::loadAtmosphere() failed to load XML");
    }

    XMLElement* atmosphereElem = root->FirstChildElement("atmosphere");

    if (atmosphereElem)
    {
        atmosphere = new Atmosphere();
        atmosphere->genBuffer(window->getRenderSize());
        
        XMLElement* iBeautyElem = atmosphereElem->FirstChildElement("ibeauty");

        if (iBeautyElem)
        {
            int iBeauty = 0;

            iBeautyElem->QueryIntAttribute("beauty", &iBeauty);

            atmosphere->setIBeauty(iBeauty);
        }
        
        XMLElement* jBeautyElem = atmosphereElem->FirstChildElement("jbeauty");

        if (jBeautyElem)
        {
            int jBeauty = 0;

            jBeautyElem->QueryIntAttribute("beauty", &jBeauty);

            atmosphere->setJBeauty(jBeauty);
        }

        /* sphere */
        XMLElement* sphereElem = atmosphereElem->FirstChildElement("sphere");

        if (sphereElem)
        {
            XMLElement* centerElem = sphereElem->FirstChildElement("center");

            vec3 center(0);

            if (centerElem)
            {
                centerElem->QueryFloatAttribute("x", &center.x);
                centerElem->QueryFloatAttribute("y", &center.y);
                centerElem->QueryFloatAttribute("z", &center.z);
            }

            XMLElement* radiusElem = sphereElem->FirstChildElement("radius");

            float radius = 0;

            if (radiusElem)
            {
                radiusElem->QueryFloatAttribute("radius", &radius);
            }

            XMLElement* qualityElem = sphereElem->FirstChildElement("quality");

            int quality = 0;

            if (qualityElem)
            {
                qualityElem->QueryIntAttribute("quality", &quality);
            }

            atmosphere->genDome(center, radius, quality);
        }


        XMLElement* rayOriginElem = atmosphereElem->FirstChildElement("rayorigin");

        if (rayOriginElem)
        {
            vec3 rayOrigin(0.0);

            rayOriginElem->QueryFloatAttribute("x", &rayOrigin.x);
            rayOriginElem->QueryFloatAttribute("y", &rayOrigin.y);
            rayOriginElem->QueryFloatAttribute("z", &rayOrigin.z);

            atmosphere->setRayOrigin(rayOrigin);
        }
        
        XMLElement* sunPosElem = atmosphereElem->FirstChildElement("sunpos");

        if (sunPosElem)
        {
            vec3 sunPos(0.0);

            sunPosElem->QueryFloatAttribute("x", &sunPos.x);
            sunPosElem->QueryFloatAttribute("y", &sunPos.y);
            sunPosElem->QueryFloatAttribute("z", &sunPos.z);

            atmosphere->setSunPos(sunPos);
        }
        
        XMLElement* sunIntensityElem = atmosphereElem->FirstChildElement("sunintensity");

        if (sunIntensityElem)
        {
            float sunIntensity = 0.0;

            sunIntensityElem->QueryFloatAttribute("intensity", &sunIntensity);

            atmosphere->setSunIntensity(sunIntensity);
        }
        
        XMLElement* planetRadiusElem = atmosphereElem->FirstChildElement("planetradius");

        if (planetRadiusElem)
        {
            float planetRadius = 0.0;

            planetRadiusElem->QueryFloatAttribute("radius", &planetRadius);

            atmosphere->setPlanetRadius(planetRadius);
        }
        
        XMLElement* atmoRadiusElem = atmosphereElem->FirstChildElement("atmoradius");

        if (atmoRadiusElem)
        {
            float atmoRadius = 0.0;

            atmoRadiusElem->QueryFloatAttribute("radius", &atmoRadius);

            atmosphere->setAtmoRadius(atmoRadius);
        }
        
        XMLElement* rayleighCoeffElem = atmosphereElem->FirstChildElement("rayleighcoeff");

        if (rayleighCoeffElem)
        {
            vec3 rayleighCoeff(0.0);

            rayleighCoeffElem->QueryFloatAttribute("x", &rayleighCoeff.x);
            rayleighCoeffElem->QueryFloatAttribute("y", &rayleighCoeff.y);
            rayleighCoeffElem->QueryFloatAttribute("z", &rayleighCoeff.z);

            atmosphere->setRayleighCoeff(rayleighCoeff);
        }
        
        XMLElement* mieCoeffElem = atmosphereElem->FirstChildElement("miecoeff");

        if (mieCoeffElem)
        {
            float mieCoeff = 0.0;

            mieCoeffElem->QueryFloatAttribute("coeff", &mieCoeff);

            atmosphere->setMieCoeff(mieCoeff);
        }
        
        XMLElement* rayleighHeightElem = atmosphereElem->FirstChildElement("rayleighheight");

        if (rayleighHeightElem)
        {
            float rayleighHeight = 0.0;

            rayleighHeightElem->QueryFloatAttribute("height", &rayleighHeight);

            atmosphere->setRayleighHeight(rayleighHeight);
        }
        
        XMLElement* mieHeightElem = atmosphereElem->FirstChildElement("mieheight");

        if (mieHeightElem)
        {
            float mieHeight = 0.0;

            mieHeightElem->QueryFloatAttribute("height", &mieHeight);

            atmosphere->setMieHeight(mieHeight);
        }
        
        XMLElement* mieDirElem = atmosphereElem->FirstChildElement("miedir");

        if (mieDirElem)
        {
            float mieDir = 0.0;

            mieDirElem->QueryFloatAttribute("dir", &mieDir);

            atmosphere->setMieDir(mieDir);
        }
    }
}

void LevelLoader::loadSsao()
{
    XMLDocument sSAODoc;

    sSAODoc.LoadFile((levelName + "/ssao.xml").c_str());

    XMLNode* root = sSAODoc.FirstChildElement("SsaoFile");

    if (!root)
    {
        throw runtime_error("ERROR::loadSsao() failed to load XML");
    }

    XMLElement* sSAOElem = root->FirstChildElement("ssao");

    if (sSAOElem)
    {
        sSAO = new SSAO();
        sSAO->genBuffer(window->getRenderSize());

        XMLElement* kernelSizeElem = sSAOElem->FirstChildElement("kernelsize");

        if (kernelSizeElem)
        {
            int kernelSize = 0;

            kernelSizeElem->QueryIntAttribute("size", &kernelSize);

            sSAO->genSampleKernel(kernelSize);
        }
        
        XMLElement* noiseSizeElem = sSAOElem->FirstChildElement("noisesize");

        if (noiseSizeElem)
        {
            int noiseSize = 0;

            noiseSizeElem->QueryIntAttribute("size", &noiseSize);

            sSAO->genNoise(noiseSize);
        }
        
        XMLElement* blurScaleElem = sSAOElem->FirstChildElement("blurscale");

        if (blurScaleElem)
        {
            int blurScale = 1;

            blurScaleElem->QueryIntAttribute("scale", &blurScale);

            sSAO->setSoftness(blurScale);
        }
        
        XMLElement* radiusElem = sSAOElem->FirstChildElement("radius");

        if (radiusElem)
        {
            float radius = 0.0;

            radiusElem->QueryFloatAttribute("radius", &radius);

            sSAO->setRadius(radius);
        }
        
        XMLElement* biasElem = sSAOElem->FirstChildElement("bias");

        if (biasElem)
        {
            float bias = 0.0;

            biasElem->QueryFloatAttribute("bias", &bias);

            sSAO->setBias(bias);
        }
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

    projection = mat4(1.0);
    loadProjection(projElem, projection);

    viewFrustum = new ViewFrustum;
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
            GameObject* GO = nullptr;
            loadGameObject(gameObjectElem, GO);

            gameObjects.insert({GO->getName(), GO});

            player->setGameObject(GO);
        }

        /* visible */
        XMLElement* activeElem = playerElem->FirstChildElement("active");

        if (activeElem)
        {
            bool active = false;

            activeElem->QueryBoolAttribute("active", &active);

            player->setActive(active);
        }

        /* raytracer */
        XMLElement* rayTracerElem = playerElem->FirstChildElement("raytracer");

        if (rayTracerElem)
        {
            bool apply = false;

            rayTracerElem->QueryBoolAttribute("apply", &apply);

            if (apply)
            {
                RayTracer* rayTracer = new RayTracer(physicsWorld->getWorld(), nullptr, projection);     

                player->setRayTracer(rayTracer);
                rayTracer->setCamera(player);
            }
        }

        /* camera offset */
        XMLElement* cameraOffsetElem = playerElem->FirstChildElement("cameraoffset");

        if (cameraOffsetElem)
        {
            float x = 0, y = 0, z = 0;

            cameraOffsetElem->QueryFloatAttribute("x", &x);
            cameraOffsetElem->QueryFloatAttribute("y", &y);
            cameraOffsetElem->QueryFloatAttribute("z", &z);

            player->setCameraOffset(vec3(x, y, z));
        }

        /* model offset */
        XMLElement* modelOffsetElem = playerElem->FirstChildElement("modeloffset");

        if (modelOffsetElem)
        {
            float x = 0, y = 0, z = 0;

            modelOffsetElem->QueryFloatAttribute("x", &x);
            modelOffsetElem->QueryFloatAttribute("y", &y);
            modelOffsetElem->QueryFloatAttribute("z", &z);

            player->setModelOffset(vec3(x, y, z));
        }

        players.push_back(player);

        playerElem = playerElem->NextSiblingElement();
    }

    if (players.empty())
    {
        throw runtime_error("ERROR::loadPlayers() at least 1 player is required");
    }
}

void LevelLoader::loadSoldiers()
{
    XMLDocument soldierDoc;

    soldierDoc.LoadFile((levelName + "/soldier.xml").c_str());

    XMLNode* root = soldierDoc.FirstChildElement("SoldierFile");

    if (!root)
    {
        throw runtime_error("ERROR::loadSoldiers() failed to load XML");
    }

    XMLNode* soldierNode = root->FirstChildElement("soldiers"); 
    XMLElement* soldierElem = soldierNode->FirstChildElement("soldier");

    while (soldierElem)
    {
        /* position */
        XMLElement* positionElem = soldierElem->FirstChildElement("position");
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
        XMLElement* forwardElem = soldierElem->FirstChildElement("forward");
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
        XMLElement* speedElem = soldierElem->FirstChildElement("speed");
        float speed = 1.0;

        if (speedElem)
        {
            speedElem->QueryFloatAttribute("speed", &speed);
        }

        Soldier* soldier = new Soldier(window, position, forward, speed);

        /* game object */
        XMLElement* gameObjectElem = soldierElem->FirstChildElement("gameobject");

        if (gameObjectElem)
        {
            GameObject* GO = nullptr;
            loadGameObject(gameObjectElem, GO);

            gameObjects.insert({GO->getName(), GO});

            soldier->setGameObject(GO);
        }

        /* visible */
        XMLElement* activeElem = soldierElem->FirstChildElement("active");

        if (activeElem)
        {
            bool active = false;

            activeElem->QueryBoolAttribute("active", &active);

            soldier->setActive(active);
        }

        /* raytracer */
        XMLElement* rayTracerElem = soldierElem->FirstChildElement("raytracer");

        if (rayTracerElem)
        {
            bool apply = false;

            rayTracerElem->QueryBoolAttribute("apply", &apply);

            if (apply)
            {
                RayTracer* rayTracer = new RayTracer(physicsWorld->getWorld(), nullptr, projection);     

                soldier->setRayTracer(rayTracer);
                rayTracer->setCamera(soldier);
            }
        }

        /* camera offset */
        XMLElement* cameraOffsetElem = soldierElem->FirstChildElement("cameraoffset");

        if (cameraOffsetElem)
        {
            float x = 0, y = 0, z = 0;

            cameraOffsetElem->QueryFloatAttribute("x", &x);
            cameraOffsetElem->QueryFloatAttribute("y", &y);
            cameraOffsetElem->QueryFloatAttribute("z", &z);

            soldier->setCameraOffset(vec3(x, y, z));
        }

        /* model offset */
        XMLElement* modelOffsetElem = soldierElem->FirstChildElement("modeloffset");

        if (modelOffsetElem)
        {
            float x = 0, y = 0, z = 0;

            modelOffsetElem->QueryFloatAttribute("x", &x);
            modelOffsetElem->QueryFloatAttribute("y", &y);
            modelOffsetElem->QueryFloatAttribute("z", &z);

            soldier->setModelOffset(vec3(x, y, z));
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

                Weapon* weapon = dynamic_cast < Weapon* >(gameObjects.find(name)->second);

                if (!weapon)
                {
                    throw runtime_error("ERROR::loadSoldiers() can't find the weapon");
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

    loadProjection();

    loadSsao();
    loadAtmosphere();
    loadSkyBox();
    loadDirLight();
    loadGameObjects();

    loadRifles();

    loadSoldiers();
    loadPlayers();
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
        
void LevelLoader::getAtmosphereData(Atmosphere*& atmosphere) const
{
    atmosphere = this->atmosphere;
}
        
void LevelLoader::getSsaoData(SSAO*& sSAO) const
{
    sSAO = this->sSAO;
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
