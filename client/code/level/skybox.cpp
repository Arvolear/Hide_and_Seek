#include "../shader/shader.hpp" 

#include "skybox.hpp"

using namespace std;
using namespace glm;

SkyBox::SkyBox()
{
    VAO = 0;
    VBO = 0;
    textureID = 0;
}

void SkyBox::init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyBoxVertices), &skyBoxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void SkyBox::loadSkyBox(string path)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int W, H;
    
    for (size_t i = 0; i < texFaces.size(); i++)
    {
        string localPath = path + "/" + texFaces[i];

        unsigned char* image = SOIL_load_image(localPath.c_str(), &W, &H, 0, SOIL_LOAD_RGB);

        if (image)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            SOIL_free_image_data(image);
        }
        else
        {
            SOIL_free_image_data(image);
            
            throw runtime_error("ERROR::Failed to load texture at path " + localPath);
        }
    }
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void SkyBox::render(Shader* shader)
{
    glDepthFunc(GL_LEQUAL);

    shader->setInt("skybox", textureID);
    glActiveTexture(GL_TEXTURE0 + textureID); 

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}

SkyBox::~SkyBox()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &textureID);
}
