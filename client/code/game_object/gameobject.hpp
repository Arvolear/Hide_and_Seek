#pragma once

#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <memory>

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
        bool shadow;

        ModelLoader* modelLoader;

        string graphicsObject;

        vector < Mesh* > meshes;
        Skeleton* skeleton;

        map < string, Animation* > animations;

        ViewFrustum* viewFrustum;

        BoundSphere* boundSphere;
        DebugSphere* debugSphere;

        mat4 localTransform;
        mat4 modelTransform;
        
        void removeGraphicsObject();

    public:
        GameObject(string name);

        void setName(string name);
        void setVisible(bool visible);
        void setShadow(bool shadow);
        void setGraphicsObject(string path);
        void setViewFrustum(ViewFrustum* frustum);

        void setLocalRotation(vec3 axis, float angle, bool add = true);
        void setLocalScale(vec3 growth, bool add = true);
        void setLocalPosition(vec3 pos, bool add = true);
        void setLocalTransform(mat4 localTransform);
        void setModelTransform(mat4 modelTransform);
        void clearLocalTransform();

        void addAnimation(Animation* anim);
        void removeAnimation(string name);
        void playAnimation(string name, bool reset = true);
        void stopAnimation();

        string getGraphicsObject() const;
        Skeleton* getSkeleton() const;
        string getName() const;
        bool isVisible() const;
        bool isShadow() const;

        mat4 getLocalTransform() const;
        mat4 getModelTransform() const;

        Animation* getActiveAnimation() const;
        Animation* getAnimation(string name) const;

        virtual void render(Shader* shader, bool cull = true);
        
        /*** DEBUG ***/
        void createDebugSphere(int depth);
        void renderDebugSphere(Shader* shader);

        virtual ~GameObject();
};
