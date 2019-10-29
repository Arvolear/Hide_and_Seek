#pragma once

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

#define BONES_AMOUNT 6 

class Mesh
{
    public:
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

    private:
        GLuint VAO;
        GLuint instanceVBO;
        GLuint VBO, EBO;

        int instanceAmount;

        vector < Vertex > vertices; 
        vector < GLuint > indices; 
        vector < Texture > textures; 

        void setupMesh(); 

    public:
        Mesh (vector < Vertex > &v, vector < unsigned int > &i, vector < Texture > &t);

        void setupInstancedMesh(vector < mat4 > &transformations);

        void render(Shader *shader, bool instanced = false) const; 

        vector < Vertex > getVertices() const;

        ~Mesh();
};
