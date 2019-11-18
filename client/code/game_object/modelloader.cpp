#include "../global/globaluse.hpp"

#include "../shader/shader.hpp"

#include "animation.hpp"
#include "mesh.hpp"
#include "bone.hpp"
#include "skeleton.hpp"
#include "modelloader.hpp"

using namespace std;
using namespace glm;
using namespace Assimp;
        
map < string, Mesh::Texture > ModelLoader::textures_loaded; 

ModelLoader::ModelLoader(){}

void ModelLoader::loadModel(string path)
{
    scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace); 

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        throw runtime_error("ERROR::ASSIMP::" + string(aiString(import.GetErrorString()).C_Str())); 
    }

    directory = path.substr(0, path.find_last_of('/')); 
    directory += "/";
   
    processNode(scene->mRootNode); 
    processNodeAnim(); 
    processBone(); 
    processMeshes(scene->mRootNode); 

    skeleton = new Skeleton(bones);
}

void ModelLoader::getModelData(Skeleton*& skeleton, vector < Mesh* > &meshes) const
{
    skeleton = this->skeleton;
    meshes = this->meshes;
}
        
void ModelLoader::clear()
{
    bones.clear();
    meshes.clear();
    //textures_loaded.clear();
    directory = "";
    skeleton = nullptr;

    scene = nullptr;

    nodes.clear();
    
    for (size_t i = 0; i < animations.size(); i++)
    {
        for (auto &j: animations[i])
        {
            delete j.second;
        }
    }

    animations.clear();
}

void ModelLoader::processNode(aiNode *node)
{
    nodes.insert({node->mName.data, node});

    for (size_t j = 0; j < node->mNumChildren; j++) 
    {
        processNode(node->mChildren[j]);
    }
}

void ModelLoader::processNodeAnim()
{
    if (scene->mNumAnimations == 0) 
    {
        return;
    }

    for (size_t i = 0; i < scene->mNumAnimations; i++) 
    {
        map < string, AnimationData* > nodesAnim;

        for (size_t j = 0; j < scene->mAnimations[i]->mNumChannels; j++) 
        {
            AnimationData* AD = new AnimationData();

            AD->name = scene->mAnimations[i]->mName.data;
            AD->duration = scene->mAnimations[i]->mDuration;
            AD->speed = scene->mAnimations[i]->mTicksPerSecond / 120.0;
            AD->nodeAnim = scene->mAnimations[i]->mChannels[j];

            nodesAnim.insert({scene->mAnimations[i]->mChannels[j]->mNodeName.data, AD});
        }

        animations.push_back(nodesAnim);
    }
}

void ModelLoader::processBone()
{
    for (size_t i = 0; i < scene->mNumMeshes; i++) 
    {
        for (size_t j = 0; j < scene->mMeshes[i]->mNumBones; j++) 
        {
            string boneName = scene->mMeshes[i]->mBones[j]->mName.data; 
            mat4 boneOffset = global.aiMatrix4x4ToGlm(scene->mMeshes[i]->mBones[j]->mOffsetMatrix); 

            Bone* bone = new Bone(); 

            bone->setId(bones.size());
            bone->setName(boneName);
            bone->setOffset(boneOffset);
            bone->setNode(findAiNode(boneName)); 
            bone->setAnimation(findAiNodeAnims(boneName)); 

            bones.insert({bone->getName(), bone}); 
        }
    }

    for (auto& it: bones) 
    {
        string parentName = it.second->getNode()->mParent->mName.data; 

        Bone* parentBone = findBone(parentName); 

        it.second->setParentBone(parentBone); 
    }    
}

void ModelLoader::processMeshes(aiNode *node)
{
    for (size_t i = 0; i < node->mNumMeshes; i++) 
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh)); 
    }
    
    for (size_t j = 0; j < node->mNumChildren; j++) 
    {
        processMeshes(node->mChildren[j]);
    }
}

Mesh* ModelLoader::processMesh(aiMesh *mesh)
{
    vector < Mesh::Vertex > vertices; 
    vector < unsigned int > indices; 
    vector < Mesh::Texture > textures; 

    for (size_t i = 0; i < mesh->mNumVertices; i++) 
    {
        Mesh::Vertex vertex;
        vec3 helpVec3;

        helpVec3.x = mesh->mVertices[i].x; 
        helpVec3.y = mesh->mVertices[i].y;
        helpVec3.z = mesh->mVertices[i].z;

        vertex.position = helpVec3; 
        
        if (mesh->HasNormals()) 
        {
            helpVec3.x = mesh->mNormals[i].x; 
            helpVec3.y = mesh->mNormals[i].y;
            helpVec3.z = mesh->mNormals[i].z;

            vertex.normal = helpVec3; 
        }

        if (mesh->HasTangentsAndBitangents()) 
        {
            helpVec3.x = mesh->mTangents[i].x; 
            helpVec3.y = mesh->mTangents[i].y;
            helpVec3.z = mesh->mTangents[i].z;

            vertex.tangent = helpVec3; 
        }

        if (mesh->HasTextureCoords(0)) 
        {
            vec2 helpVec2;

            helpVec2.x = mesh->mTextureCoords[0][i].x; 
            helpVec2.y = mesh->mTextureCoords[0][i].y;

            vertex.texCoords = helpVec2; 
        }
        else
        {
            vertex.texCoords = vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (size_t i = 0; i < mesh->mNumBones; i++) 
    {
        aiBone* bone = mesh->mBones[i];

        for (size_t j = 0; j < bone->mNumWeights; j++) 
        {
            aiVertexWeight vertexWeight = bone->mWeights[j]; 

            int startVertexID = vertexWeight.mVertexId; 

            for (int k = 0; k < BONES_AMOUNT; k++) 
            {
                if (vertices[startVertexID].weights[k] == 0.0) 
                {
                    vertices[startVertexID].boneIDs[k] = findBoneId(bone->mName.data); 

                    vertices[startVertexID].weights[k] = vertexWeight.mWeight; 

                    break; 
                }

                if (k == BONES_AMOUNT - 1) 
                {                
                    break;
                }
            }
        }
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++) 
    {
        aiFace face = mesh->mFaces[i];

        for (size_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex]; 
    
    vector < Mesh::Texture > diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); 
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end()); 
    
    vector < Mesh::Texture > roughnessMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_roughness");
    textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
    
    vector < Mesh::Texture > specularMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_metallic"); 
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    
    vector < Mesh::Texture > aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ao");
    textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
    
    /* .obj */
    vector < Mesh::Texture > normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal"); 

    /* .fbx */
    if (normalMaps.empty())
    {
        normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal"); 
    }

    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    return new Mesh(vertices, indices, textures);
}

vector < Mesh::Texture > ModelLoader::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector < Mesh::Texture > textures; 

    for (size_t i = 0; i < mat->GetTextureCount(type); i++) 
    {
        aiString helpStr;

        mat->GetTexture(type, i, &helpStr); 

        string texPath = directory + string(helpStr.C_Str());

        auto it = textures_loaded.find(texPath);

        if (it != textures_loaded.end()) 
        {
            textures.push_back(it->second); 
        }
        else 
        {
            Mesh::Texture texture;

            texture.id = textureFromFile(texPath); 
            texture.type = typeName; 
            texture.path = texPath; 

            textures.push_back(texture); 
            textures_loaded.insert({texPath, texture}); 
        }
    }

    return textures;
}

unsigned int ModelLoader::textureFromFile(string filename)
{
    string extension = filename.substr(filename.find_last_of('.'));

    if (extension == ".dds")
    {
        return loadCompressed(filename.data());
    }
    else
    {
        return loadNotCompressed(filename);
    }
}

unsigned int ModelLoader::loadCompressed(const char* path) 
{
    unsigned char* header;

    unsigned int width;
    unsigned int height;
    unsigned int mipMapCount;

    unsigned int blockSize;
    unsigned int format;

    unsigned int w;
    unsigned int h;

    unsigned char* buffer = 0;

    GLuint tid = 0;

    FILE* f;

    if ((f = fopen(path, "rb")) == 0)
    {
        return 0;
    }
    
    fseek(f, 0, SEEK_END);
    unsigned int file_size = ftell(f); 
    fseek(f, 0, SEEK_SET);

    header = (unsigned char*)malloc(128);
    unsigned int bytesRead = fread(header, 1, 128, f);

    assert(bytesRead == 128);

    if (memcmp(header, "DDS ", 4) != 0)
    {
        free(buffer);
        free(header);
        fclose(f);
        return tid;
    }

    height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
    width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
    mipMapCount = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

    //cout << height << ' ' << width << ' ' << mipMapCount << endl;

    if(header[84] == 'D') 
    {
        switch(header[87]) 
        {
            case '1': // DXT1
                {
                    format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                    blockSize = 8;
                    break;
                }
            case '3': // DXT3
                {
                    format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                    blockSize = 16;
                    break;
                }
            case '5': // DXT5
                {
                    format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                    blockSize = 16;
                    break;
                }
            default: 
                {
                    free(buffer);
                    free(header);
                    fclose(f);
                    return tid;
                }
        }
    } 
    else
    {
        free(buffer);
        free(header);
        fclose(f);
        return tid;
    }

    buffer = (unsigned char*)malloc(file_size - 128);

    if (buffer == 0)
    {
        free(buffer);
        free(header);
        fclose(f);
        return tid;
    }
    
    bytesRead = fread(buffer, 1, file_size, f);
    
    assert(bytesRead == file_size - 128);

    glGenTextures(1, &tid);
    
    if (tid == 0)
    {
        free(buffer);
        free(header);
        fclose(f);
        return tid;
    }

    glBindTexture(GL_TEXTURE_2D, tid);
        
    glGenerateMipmap(GL_TEXTURE_2D); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    unsigned int offset = 0;
    unsigned int size = 0;
    w = width;
    h = height;

    for (unsigned int i = 0; i < mipMapCount; i++) 
    {
        size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;

        glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer + offset);

        offset += size;
        w /= 2;
        h /= 2;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    free(buffer);
    free(header);
    fclose(f);
    return tid;
}

unsigned int ModelLoader::loadNotCompressed(string filename)
{
    unsigned int textureID;
    glGenTextures(1, &textureID); 
    glBindTexture(GL_TEXTURE_2D, textureID); 

    int W, H; 
    unsigned char* image = SOIL_load_image(filename.data(), &W, &H, 0, SOIL_LOAD_RGBA); 

    if (image) 
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, image); 

        glGenerateMipmap(GL_TEXTURE_2D); 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SOIL_free_image_data(image); 
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);

        throw runtime_error("ERROR::Failed to load texture at path: " + filename);
    } 

    return textureID;
}

Bone* ModelLoader::findBone(string name) const
{
    map < string, Bone* >::const_iterator it;
    it = bones.find(name);

    if (it != bones.end())
    {
        return it->second;
    }

    return 0;
}

int ModelLoader::findBoneId(string name) const
{
    map < string, Bone* >::const_iterator it;
    it = bones.find(name);

    if (it != bones.end())
    {
        return it->second->getId();
    }

    return -1;
}

aiNode* ModelLoader::findAiNode(string name) const
{
    map < string, aiNode* >::const_iterator it;
    it = nodes.find(name);

    if (it != nodes.end()) 
    {
        return it->second; 
    }

    return 0;
}

vector < AnimationData* > ModelLoader::findAiNodeAnims(string name) const
{
    vector < AnimationData* > animation;

    for (size_t i = 0; i < animations.size(); i++)
    {
        map < string, AnimationData* >::const_iterator it;
        it = animations[i].find(name);

        if (it != animations[i].end()) 
        {
            animation.push_back(it->second); 
        }
    }

    return animation;
}

ModelLoader::~ModelLoader()
{
    clear();
}
