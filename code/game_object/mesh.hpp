#pragma once

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

#define BONES_AMOUNT 6 

struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texCoords; 
    vec3 tangent;

    float boneIDs[BONES_AMOUNT] = {0.0f}; 
    float weights[BONES_AMOUNT] = {0.0f}; 
};

struct Texture
{
    unsigned int id; 
    string type; 
    string path; 
};

class Mesh
{
    private:
        GLuint VAO;
        GLuint VBO, EBO; 

        vector < Vertex > vertices; 
        vector < GLuint > indices; 
        vector < Texture > textures; 
        
        void setupMesh(); 
        
    public:
        Mesh (vector < Vertex > &v, vector < unsigned int > &i, vector < Texture > &t);

        void draw(Shader *shader) const; 

        vector < Vertex > getVertices() const;

        ~Mesh();
};
