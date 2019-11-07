#pragma once

#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <memory>
#include <mutex>
#include <condition_variable>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

using namespace std;
using namespace glm;

class GameObject
{
    protected:
        /* all object names are stored here */
        static set < string > globalNames;
        string name;
        bool visible;
        bool viewStatic;
        bool shadow;
        bool cull;

        float minNormalCosAngle;

        ModelLoader* modelLoader;

        string graphicsObject;
        PhysicsObject* physicsObject;

        vector < Mesh* > meshes;
        Skeleton* skeleton;

        map < string, Animation* > animations;

        ViewFrustum* viewFrustum;

        BoundSphere* boundSphere;
        Sphere* sphere;

        mat4 localTransform;

        float interpolationCoeff;
        bool interpolation;
        mat4 nextTransform;
        mat4 prevTransform;

        void* userPointer;

        /* multiplayer */
        bool ready;
        mutex mtx;
        condition_variable cv;
        
        void removePhysicsObject();
        void removeGraphicsObject();

    public:
        GameObject(string name);

        void setName(string name);
        void setCull(bool cull);
        void setMinNormalCosAngle(float minNormalCosAngle);
        void setVisible(bool visible);
        void setViewStatic(bool viewStatic);
        void setShadow(bool shadow);
        void setCollidable(bool collidable);
        void setStatic(bool stat);
        void setGraphicsObject(string path);
        void setViewFrustum(ViewFrustum* frustum);
        void setPhysicsObject(PhysicsObject* object);

        virtual void createBoundSphere();

        void setLocalRotation(vec3 axis, float angle, bool add = true);
        void setLocalScale(vec3 growth, bool add = true);
        void setLocalPosition(vec3 pos, bool add = true);
        void setLocalTransform(mat4 localTransform);
        void clearLocalTransform();
        void clearPhysicsObjectTransform();

        void setPhysicsObjectTransform(mat4 model, bool interpolation = false);

        void setUserPointer(void* userPointer);
        
        void addAnimation(Animation* anim);
        void removeAnimation(string name);
        void playAnimation(string name, bool reset = true);
        void stopAnimation();

        PhysicsObject* getPhysicsObject() const;
        string getGraphicsObject() const;
        Skeleton* getSkeleton() const;
        string getName() const;
        bool isCull() const;
        bool isVisible() const;
        bool isViewStatic() const;
        bool isShadow() const;
        bool isCollidable() const;
        bool isStatic() const;

        mat4 getLocalTransform() const;
        mat4 getPhysicsObjectTransform() const;

        void* getUserPointer() const;

        Animation* getActiveAnimation() const;
        Animation* getAnimation(string name) const;

        virtual void render(Shader* shader, bool viewCull = true);
        
        /*** DEBUG ***/
        void createDebugSphere(int depth);
        void renderDebugSphere(Shader* shader);

        virtual ~GameObject();
};
