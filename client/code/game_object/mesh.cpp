#include "../shader/shader.hpp"

#include "mesh.hpp"

using namespace std;
using namespace glm;

Mesh::Mesh(vector < Vertex > &v, vector < unsigned int > &i, vector < Texture > &t)
{
    vertices = v; 
    indices = i; 
    textures = t; 
    
    instanceAmount = 0;

    setupMesh(); 
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &VBO); 
    glGenBuffers(1, &EBO); 

    glBindVertexArray(VAO); 

    glBindBuffer(GL_ARRAY_BUFFER, VBO); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // bind EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW); // insert index data into EBO

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0); // 0 layout for position

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1); // 1 layout for normal

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2); // 2 layout for UV
    
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3); // 3 layout for tangent
   
    for (int i = 0; i < BONES_AMOUNT / 2; i++)
    {
        glVertexAttribPointer(4 + i, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, boneIDs) + sizeof(float) * 2 * i));
        glEnableVertexAttribArray(4 + i); // from 4 to 7 (vec2 BONES_AMOUNT)
    }
    
    for (int i = 0; i < BONES_AMOUNT / 2; i++)
    {
        glVertexAttribPointer(7 + i, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, weights) + sizeof(float) * 2 * i));
        glEnableVertexAttribArray(7 + i); // from 7 to 10 for the weights
    }

    glBindVertexArray(0); 
}
        
void Mesh::setupInstancedMesh(vector < mat4 > &transformations)
{
    instanceAmount = transformations.size();

    glGenBuffers(1, &instanceVBO); 

    glBindVertexArray(VAO); 
    
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * transformations.size(), transformations.data(), GL_STATIC_DRAW); 

    for (int i = 0; i < 4; i++)
    {
        glVertexAttribPointer(10 + i, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void*)(sizeof(vec4) * i));
        glEnableVertexAttribArray(10 + i);
        glVertexAttribDivisor(10 + i, 1);
    }
    
    glBindVertexArray(0); 
}

void Mesh::render(Shader *shader, bool instanced) const
{
    unsigned int normalNR = 1;
    unsigned int diffuseNR = 1;
    unsigned int metallicNR = 1;
    unsigned int roughnessNR = 1;
    unsigned int aoNR = 1; 
        
    for (size_t i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        string number;
        
        if (textures[i].type == "texture_normal")
        {
            number = to_string(normalNR++);
        }
        else if (textures[i].type == "texture_diffuse")
        {
            number = to_string(diffuseNR++);
        }
        else if (textures[i].type == "texture_metallic")
        {
            number = to_string(metallicNR++);
        }
        else if (textures[i].type == "texture_roughness")
        {
            number = to_string(roughnessNR++);
        }
        else if (textures[i].type == "texture_ao")
        {
            number = to_string(aoNR++);
        }

        //cout << "material." + textures[i].type + number << endl;
        //cout << textures[i].id << endl;

        shader->setInt("material." + textures[i].type + number, i); // send the texture to the shader (example: material.texture_diffuse1)
        glBindTexture(GL_TEXTURE_2D, textures[i].id); // bind this texture
    }

    if (normalNR == 1)
    {
        shader->setInt("meshNormalMapped", 0);
    }
    else
    {
        shader->setInt("meshNormalMapped", 1); 
    }

    glBindVertexArray(VAO); // bind VAO

    if (!instanced)
    {
        shader->setInt("meshInstanced", 0);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // draw mesh from indices 
    }
    else
    {
        shader->setInt("meshInstanced", 1);
        glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceAmount);
    }

    glBindVertexArray(0); // unbind VAO
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

vector < Vertex > Mesh::getVertices() const
{
    return vertices;
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    for (size_t i = 0; i < textures.size(); i++)
    {
        glDeleteTextures(1, &textures[i].id);
    }
}
