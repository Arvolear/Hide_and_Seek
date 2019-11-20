#pragma once

#include <vector>
#include <algorithm>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <map>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <SOIL/SOIL.h>

#include <nv_dds/nv_dds.h>

using namespace nv_dds;
using namespace std;
using namespace glm;
using namespace Assimp;

class ModelLoader
{
    private:
        map < string, Bone* > bones; 
        vector < Mesh* > meshes; 
        static map < string, Mesh::Texture > textures_loaded; 
        string directory; 
        Skeleton *skeleton;

        Importer import; 
        const aiScene* scene; 
        
        map < string, aiNode* > nodes; 
        vector < map < string, AnimationData* > > animations; 

        Window* window;

        mutex mtx;
        mutex mtx2;
        mutex mtx3;

        void processNode(aiNode *node); 
        void processNodeAnim(); 
        void processBone(); 
        void processMeshes(aiNode *node); 
        Mesh* processMesh(aiMesh *mesh); 

        void loadDiffNorm(aiMaterial* material, vector < Mesh::Texture > &textures);
        void loadRoughMetAO(aiMaterial* material, vector < Mesh::Texture > &textures);
        vector < Mesh::Texture > loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName); 
        unsigned int textureFromFile(string filename);
        unsigned int loadCompressed(string filename);
        unsigned int loadNotCompressed(string filename);

        Bone* findBone(string name) const; 
        int findBoneId(string name) const; 
        aiNode* findAiNode(string name) const; 
        vector < AnimationData* > findAiNodeAnims(string name) const; 

    public:
        ModelLoader(Window* window);

        void loadModel(string path); 

        void getModelData(Skeleton *&skeleton, vector < Mesh* > &meshes) const; 

        void clear();

        ~ModelLoader();
};
