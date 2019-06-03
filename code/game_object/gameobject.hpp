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

        btDynamicsWorld* world;
        ModelLoader* modelLoader;

        string graphicsObject;
        PhysicsObject* physicsObject;

        vector < Mesh* > meshes;
        Skeleton* skeleton;

        ViewFrustum* viewFrustum;

        BoundSphere* boundSphere;
        DebugSphere* debugSphere;

        mat4 localTransform;
        
        void removePhysicsObject();
        void removeGraphicsObject();
        mat4 getPhysicsObjectTransform() const;

    public:
        GameObject(string name);
        GameObject(string name, string path, PhysicsObject* physicsObject, btDynamicsWorld* world, ViewFrustum* viewFrustum);

        void setName(string name);
        void setGraphicsObject(string path);
        void setViewFrustum(ViewFrustum* frustum);
        void setPhysicsObject(PhysicsObject* object);
        void setWorld(btDynamicsWorld* world);
        
        void setLocalRotation(float angle, vec3 axis);
        void setLocalScale(vec3 growth);
        void setLocalPosition(vec3 pos);

        void playAnimation(Animation* anim, bool reset = true);
        void stopAnimation();

        PhysicsObject* getPhysicsObject() const;
        string getGraphicsObject() const;
        string getName() const;

        virtual void render(Shader* shader, bool check = true);
        
        /*** DEBUG ***/
        void createDebugSphere(int depth);
        void renderDebugSphere(mat4 &view, mat4 &projection, Shader* shader);

        virtual ~GameObject();
};
