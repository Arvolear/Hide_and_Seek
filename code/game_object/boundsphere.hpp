#pragma once

#include <vector>
#include <ctime>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;
        
class BoundSphere
{
    private:
        pair < vec3, vec3 > points;

        vector < vector < Vertex > > allVertices;

        mat4 transform;
    
        pair < vec3, vec3 > findFurthest(pair < vec3, vec3 > centerAndBound);
        vec3 findCenter();

    public:
        BoundSphere(vector < Mesh* > &meshes);

        void construct();

        void applyTransform(mat4 &matrix); //localTransform, model

        vec3 getCenter();
        double getRadius();
        
        vec3 getTransformedCenter();
        double getTransformedRadius();

        ~BoundSphere();
};
