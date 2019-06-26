#pragma once

#include <string>
#include <vector>
#include <set>
#include <algorithm>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace std;
using namespace glm;

class GameObject
{
    protected:
        /* all object names are stored here */
        static set < string > globalNames;
        string name;
        bool visible;

        ModelLoader* modelLoader;

        string graphicsObject;
        PhysicsObject* physicsObject;

        vector < Mesh* > meshes;
        Skeleton* skeleton;

        map < string, Animation* > animations;

        ViewFrustum* viewFrustum;

        BoundSphere* boundSphere;
        DebugSphere* debugSphere;

        mat4 localTransform;
        
        void removePhysicsObject();
        void removeGraphicsObject();
        mat4 getPhysicsObjectTransform() const;

    public:
        GameObject(string name);

        void setName(string name);
        void setVisible(bool visible);
        void setCollidable(bool collidable);
        void setGraphicsObject(string path);
        void setViewFrustum(ViewFrustum* frustum);
        void setPhysicsObject(PhysicsObject* object);

        void setLocalRotation(vec3 axis, float angle, bool add = true);
        void setLocalScale(vec3 growth, bool add = true);
        void setLocalPosition(vec3 pos, bool add = true);
        void clearLocalTransform();
        
        void addAnimation(Animation* anim);
        void playAnimation(string name, bool reset = true);
        void stopAnimation();

        PhysicsObject* getPhysicsObject() const;
        string getGraphicsObject() const;
        string getName() const;
        bool isVisible() const;
        bool isCollidable() const;

        Animation* getActiveAnimation() const;
        Animation* getAnimation(string name) const;

        virtual void render(Shader* shader, bool cull = true);
        
        /*** DEBUG ***/
        void createDebugSphere(int depth);
        void renderDebugSphere(Shader* shader);

        virtual ~GameObject();
};
