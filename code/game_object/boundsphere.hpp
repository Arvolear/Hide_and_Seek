#pragma once

#include <vector>
#include <ctime>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;
        
class BoundSphere
{
    private:
        pair < vec3, vec3 > centerAndBound;

        vector < vector < Vertex > > allVertices;

        mat4 transform;
    
        vec3 findFurthest(vec3 point);
        vec3 findCenter();

    public:
        BoundSphere(vector < Mesh* > &meshes);

        void construct();

        void applyTransform(mat4 &matrix); //localTransform, model

        vec3 getCenter() const;
        float getRadius() const;
        
        vec3 getTransformedCenter() const;
        float getTransformedRadius() const;

        ~BoundSphere();
};
