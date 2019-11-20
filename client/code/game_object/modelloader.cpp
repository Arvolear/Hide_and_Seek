#include "../global/globaluse.hpp"

#include "../shader/shader.hpp"

#include "../window/renderquad.hpp"
#include "../window/glfwevents.hpp"
#include "../window/window.hpp"

#include "animation.hpp"
#include "mesh.hpp"
#include "bone.hpp"
#include "skeleton.hpp"
#include "modelloader.hpp"

using namespace std;
using namespace glm;
using namespace Assimp;
        
map < string, Mesh::Texture > ModelLoader::textures_loaded; 

ModelLoader::ModelLoader(Window* window)
{
    this->window = window;
}

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
    
    window->detachCurrentContext();

    thread diffNormLoader(&ModelLoader::loadDiffNorm, this, material, ref(textures));
    thread roughMetAOLoader(&ModelLoader::loadRoughMetAO, this, material, ref(textures));
    
    //loadDiffNorm(material, textures);
    //loadRoughMetAO(material, textures);
    
    diffNormLoader.join();
    roughMetAOLoader.join();
    
    window->makeCurrentContext();

    return new Mesh(vertices, indices, textures);
}
        
void ModelLoader::loadDiffNorm(aiMaterial* material, vector < Mesh::Texture > &textures)
{
    vector < Mesh::Texture > diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); 
    
    /* .obj */
    vector < Mesh::Texture > normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal"); 

    /* .fbx */
    if (normalMaps.empty())
    {
        normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal"); 
    }

    unique_lock < mutex > lk(mtx);

    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end()); 
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
}
        
void ModelLoader::loadRoughMetAO(aiMaterial* material, vector < Mesh::Texture > &textures)
{
    vector < Mesh::Texture > roughnessMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_roughness");
    
    vector < Mesh::Texture > specularMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_metallic"); 
    
    vector < Mesh::Texture > aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ao");
    
    unique_lock < mutex > lk(mtx);

    textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
}

vector < Mesh::Texture > ModelLoader::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector < Mesh::Texture > textures; 

    for (size_t i = 0; i < mat->GetTextureCount(type); i++) 
    {
        aiString helpStr;

        mat->GetTexture(type, i, &helpStr); 

        string texPath = directory + string(helpStr.C_Str());
        
        unique_lock < mutex > lk(mtx2);
        auto it = textures_loaded.find(texPath);
        lk.unlock();

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

            lk.lock();
            textures_loaded.insert({texPath, texture}); 
            lk.unlock();
        }
    }

    return textures;
}

unsigned int ModelLoader::textureFromFile(string filename)
{
    string extension = filename.substr(filename.find_last_of('.'));

    if (extension == ".dds")
    {
        return loadCompressed(filename);
    }
    else
    {
        return loadNotCompressed(filename);
    }
}

unsigned int ModelLoader::loadCompressed(string filename) 
{
    CDDSImage image;
    image.load(filename);
    
    unique_lock < mutex > lk(mtx3);
    window->makeCurrentContext();

    unsigned int textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID); 

    glCompressedTexImage2D(GL_TEXTURE_2D, 0, image.get_format(), image.get_width(), image.get_height(), 0, image.get_size(), image);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, image.get_num_mipmaps());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    for (unsigned int i = 0; i < image.get_num_mipmaps(); i++)
    {
        CSurface mipmap = image.get_mipmap(i);

        glCompressedTexImage2D(GL_TEXTURE_2D, i + 1, image.get_format(), mipmap.get_width(), mipmap.get_height(), 0, mipmap.get_size(), mipmap);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    window->detachCurrentContext();

    return textureID;
}

unsigned int ModelLoader::loadNotCompressed(string filename)
{
    unique_lock < mutex > lk(mtx3);
    window->makeCurrentContext();
    
    int W, H; 
    unsigned char* image = SOIL_load_image(filename.data(), &W, &H, 0, SOIL_LOAD_RGBA); 

    unsigned int textureID;
    glGenTextures(1, &textureID); 
    glBindTexture(GL_TEXTURE_2D, textureID); 

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
    
    window->detachCurrentContext();

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
