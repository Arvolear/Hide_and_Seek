#include "../shader/shader.hpp"

#include "mesh.hpp"

using namespace std;
using namespace glm;

Mesh::Mesh(vector < Vertex > &v, vector < unsigned int > &i, vector < Texture > &t)
{
    vertices = v; 
    indices = i; 
    textures = t; 

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
    glEnableVertexAttribArray(3); // 2 layout for tangent
   
    for (int i = 0; i < BONES_AMOUNT; i++)
    {
        glVertexAttribPointer(4 + i, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, boneIDs) + sizeof(float) * i));
        glEnableVertexAttribArray(4 + i); // from 3 to 9 (BONES_AMOUNT) layouts for bones ids. Array in shader uses N layouts, equal to the size, instead of single layout location the vec uses
    }
    
    for (int i = 0; i < BONES_AMOUNT; i++)
    {
        glVertexAttribPointer(10 + i, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, weights) + sizeof(float) * i));
        glEnableVertexAttribArray(10 + i); // from 9 to 15 for the weights
    }

    glBindVertexArray(0); 
}

void Mesh::render(Shader *shader) const
{
    unsigned int diffuseNR = 1; // amount of diffuse textures
    unsigned int specularNR = 1; // amount of specular textures
    unsigned int normalNR = 1; // amount of normal textures
        
    shader->setFloat("material.shininess", 0.06f);

    for (size_t i = 0; i < textures.size(); i++) // loop through textures
    {
        glActiveTexture(GL_TEXTURE0 + textures[i].id); // set the texture active

        string number; // this one is needed if we use more than one texture of the same type
        
        if (textures[i].type == "texture_diffuse") // if it is a diffuse texture
        {
            number = to_string(diffuseNR++); // this is the diffuseNR`s texture
        }
        else if (textures[i].type == "texture_specular")
        {
            number = to_string(specularNR++); // this is the specularNR`s texture
        }
        else if (textures[i].type == "texture_normal")
        {
            number = to_string(normalNR++); // this is the normalNR`s texture
        }

        //cout << "material." + textures[i].type + number << endl;
        //cout << textures[i].id << endl;

        glBindTexture(GL_TEXTURE_2D, textures[i].id); // bind this texture
        shader->setInt("material." + textures[i].type + number, textures[i].id); // send the texture to the shader (example: material.texture_diffuse1)
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
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // draw mesh from indices 
    glBindVertexArray(0); // unbind VAO
    
    for (size_t i = 0; i < textures.size(); i++) // loop through textures
    {
        glActiveTexture(GL_TEXTURE0 + textures[i].id);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
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
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    for (size_t i = 0; i < textures.size(); i++)
    {
        glDeleteTextures(1, &textures[i].id);
    }
}
