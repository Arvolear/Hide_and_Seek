#include "../global/shader.hpp"
#include "../global/convert.hpp"
#include "animation.hpp"
#include "mesh.hpp"
#include "bone.hpp"
#include "skeleton.hpp"
#include "modelloader.hpp"

using namespace std;
using namespace glm;
using namespace Assimp;

ModelLoader::ModelLoader(){}

void ModelLoader::loadModel(string path)
{
    scene = import.ReadFile(path, aiProcess_Triangulate); //assimp loads the file

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) //if something gone wrong
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl; //error
        return;
    }

    directory = path.substr(0, path.find_last_of('/')); //get only the directory from the whole path to the file
   
    processNode(scene->mRootNode); //fill the nodes vector
    processNodeAnim(); //fill the nodesAnim vector
    processBone(); //get bones
    processMeshes(scene->mRootNode); //get meshes

    skeleton = new Skeleton(bones);
}

void ModelLoader::getModelData(Skeleton *&skeleton, vector < Mesh* > &meshes)
{
    skeleton = this->skeleton;
    meshes = this->meshes;
}
        
void ModelLoader::clear()
{
    bones.clear();
    meshes.clear();
    textures_loaded.clear();
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

    for (size_t j = 0; j < node->mNumChildren; j++) //recursive loop through each child to fill the vector
    {
        processNode(node->mChildren[j]);
    }
}

void ModelLoader::processNodeAnim()
{
    if (scene->mNumAnimations == 0) //if there are no animations 
    {
        return;
    }

    //cout << scene->mNumAnimations << endl;

    for (size_t i = 0; i < scene->mNumAnimations; i++) //loop through the animation to get each bone animation data
    {
        map < string, AnimationData* > nodesAnim;

        for (size_t j = 0; j < scene->mAnimations[i]->mNumChannels; j++) //loop through the animation to get each bone animation data
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
    for (size_t i = 0; i < scene->mNumMeshes; i++) //loop through each mesh of the model
    {
        for (size_t j = 0; j < scene->mMeshes[i]->mNumBones; j++) //loop through each bone which is connected to that mesh
        {
            string boneName = scene->mMeshes[i]->mBones[j]->mName.data; //get bone`s name
            mat4 boneOffset = aiMatrix4x4ToGlm(scene->mMeshes[i]->mBones[j]->mOffsetMatrix); //get bone`s offset matrix. This is the matrix which transforms from the mesh space to the bone space

            Bone* bone = new Bone(); //make new bone from the data we got (index, name, offset)

            bone->setId(bones.size());
            bone->setName(boneName);
            bone->setOffset(boneOffset);
            bone->setNode(findAiNode(boneName)); //each bone has its node with same name
            bone->setAnimation(findAiNodeAnims(boneName)); //same with the animation node

            if (!bone->getAnimation(0)) //if there is no animations for the bode. This often happens with root bones
            {
                cout << "ERROR::NO ANIMATIONS FOUND FOR " << boneName << endl;
            }

            bones.insert({bone->getName(), bone}); //push back the bone
        }
    }

    for (auto& it: bones) //here we are looking for the parent bone for our bones
    {
        string parentName = it.second->getNode()->mParent->mName.data; //get the bone parent name. For each aiNode assimp keeps the parent pointer, as the bone has the same name as it`s aiNode we can do like that

        Bone* parentBone = findBone(parentName); //find the parent bone by it`s name

        it.second->setParentBone(parentBone); //set the parent bone for the bone

        if (!parentBone) //if there is no parent bone
        {
            cout << "NO PARENT BONE FOR " << it.second->getName() << endl;
        }
    }

    //done
}

void ModelLoader::processMeshes(aiNode *node)
{
    //we start with the root node
    for (size_t i = 0; i < node->mNumMeshes; i++) //loop through each mesh this node has
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; //get the mesh from it`s index
        meshes.push_back(processMesh(mesh)); //here we call another function to extract the data and then push_back the complete mesh
    }

    for (size_t j = 0; j < node->mNumChildren; j++) //recursive loop through the each node
    {
        processMeshes(node->mChildren[j]);
    }
}

Mesh* ModelLoader::processMesh(aiMesh *mesh)
{
    vector < Vertex > vertices; //meshes vertices (position, normal, texture coords, bone ids, bone weights)
    vector < unsigned int > indices; //vertex indices for EBO
    vector < Texture > textures; //textures (id, type, path)

    for (size_t i = 0; i < mesh->mNumVertices; i++) //loop through each vertex in the mesh
    {
        Vertex vertex;
        vec3 helpVec3;

        helpVec3.x = mesh->mVertices[i].x; //positions
        helpVec3.y = mesh->mVertices[i].y;
        helpVec3.z = mesh->mVertices[i].z;

        vertex.position = helpVec3; //set position
        
        helpVec3.x = mesh->mNormals[i].x; //normals
        helpVec3.y = mesh->mNormals[i].y;
        helpVec3.z = mesh->mNormals[i].z;

        vertex.normal = helpVec3; //set normal


        if (mesh->mTextureCoords[0]) //if there is a texture
        {
            vec2 helpVec2;

            helpVec2.x = mesh->mTextureCoords[0][i].x; //textures coords
            helpVec2.y = mesh->mTextureCoords[0][i].y;

            vertex.texCoords = helpVec2; //set texture coords
        }
        else
        {
            vertex.texCoords = vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    //we have loaded the vertex data, its time for the bone data

    for (size_t i = 0; i < mesh->mNumBones; i++) //loop through each bone that mesh has
    {
        aiBone* bone = mesh->mBones[i];

        for (size_t j = 0; j < bone->mNumWeights; j++) //loop through the each bone`s vertex it carries in that mesh
        {
            aiVertexWeight vertexWeight = bone->mWeights[j]; //get the id and the weight of the carried vertex

            int startVertexID = vertexWeight.mVertexId; //get the carried vertex id

            for (int k = 0; k < BONES_AMOUNT; k++) //BONES_AMOUNT is a constant that is for the maximum amount of the bones per vertex
            {
                if (vertices[startVertexID].weights[k] == 0.0) //if we have an empty space for one more bone in the vertex
                {
                    vertices[startVertexID].boneIDs[k] = findBoneId(bone->mName.data); //set bone index to the vertex it carries

                    vertices[startVertexID].weights[k] = vertexWeight.mWeight; //set bone weight/strength to the vertex it carries

                    break; //only one place for the single bone
                }

                if (k == BONES_AMOUNT - 1) //if we have more that enough bones
                {
                    //cout << "ERROR::LOADING MORE THAN " << BONES_AMOUNT << " BONES\n"; //this could take a lot of time
                    break;
                }
            }
        }
    }

    //we have loaded everything for the vertices

    for (size_t i = 0; i < mesh->mNumFaces; i++) //loop through the mesh`s planes/faces
    {
        aiFace face = mesh->mFaces[i];

        //if face.mNumIndices == 3 we are loading triangles
        for (size_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    //we have loaded indices, loading materials

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex]; //get the material by it`s index

    vector < Texture > diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); //calling another function to load diffuse textures
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end()); //instead of the loop we can insert vector like that

    vector < Texture > specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular"); //calling another function to load specular textures. We dont have lights in this app, but if you want to add some...
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    return new Mesh(vertices, indices, textures);
}

vector < Texture > ModelLoader::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector < Texture > textures; //vector for the particular texture type

    for (size_t i = 0; i < mat->GetTextureCount(type); i++) //loop through each typed texture
    {
        aiString helpStr;

        mat->GetTexture(type, i, &helpStr); //get texture`s name

        bool skip = false;
        map < string, Texture >::iterator it;
        it = textures_loaded.find(string(helpStr.C_Str()));

        if (it != textures_loaded.end()) //if we have already loaded it
        {
            textures.push_back(it->second); //use the loaded one instead of loading the new one

            skip = true;
        }

        if (!skip) //if the texture is new
        {
            Texture texture;

            texture.id = textureFromFile(helpStr.C_Str()); //calling another function to load texture from file

            texture.type = typeName; //set the texture`s type

            cout << "Texture type: " << typeName << endl;

            texture.path = helpStr.C_Str(); //set the texture`s path

            textures.push_back(texture); //take the texture
            textures_loaded.insert({helpStr.C_Str(), texture}); //remember the texture
        }
    }

    return textures;
}

unsigned int ModelLoader::textureFromFile(const char *path)
{
    string filename = string(path); //convert to string
    filename = directory + '/' + filename; //concatenate strings to get the whole path 

    unsigned int textureID;
    glGenTextures(1, &textureID); //gen texture, opengl function
    glBindTexture(GL_TEXTURE_2D, textureID); //bind the texture

    int W, H; //width and height
    unsigned char* image = SOIL_load_image(filename.c_str(), &W, &H, 0, SOIL_LOAD_RGBA); //using SOIL to load the RGBA image

    if (image) //if the image is fine
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, image); //fill the texture with image data
        glGenerateMipmap(GL_TEXTURE_2D); //generate mipmaps 


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SOIL_free_image_data(image); //free the memory
    }
    else
    {
        cout << "Fail to load texture at path: " << path << endl;
        SOIL_free_image_data(image);
    }

    return textureID;
}

Bone* ModelLoader::findBone(string name)
{
    map < string, Bone* >::iterator it;
    it = bones.find(name);

    if (it != bones.end())
    {
        return it->second;
    }

    return 0;
}

int ModelLoader::findBoneId(string name)
{
    map < string, Bone* >::iterator it;
    it = bones.find(name);

    if (it != bones.end())
    {
        return it->second->getId();
    }

    return -1;
}

aiNode* ModelLoader::findAiNode(string name)
{
    map < string, aiNode* >::iterator it;
    it = nodes.find(name);

    if (it != nodes.end()) //if node name mathes the name
    {
        return it->second; //return node
    }

    return 0;
}

vector < AnimationData* > ModelLoader::findAiNodeAnims(string name)
{
    vector < AnimationData* > animation;

    for (size_t i = 0; i < animations.size(); i++)
    {
        map < string, AnimationData* >::iterator it;
        it = animations[i].find(name);

        if (it != animations[i].end()) //if animation node name matches the name 
        {
            animation.push_back(it->second); //return animation node
        }
    }

    return animation;
}

ModelLoader::~ModelLoader()
{
    clear();
}
