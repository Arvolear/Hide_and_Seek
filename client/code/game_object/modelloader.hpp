#pragma once

#include <vector>
#include <algorithm>
#include <string>
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

using namespace std;
using namespace glm;
using namespace Assimp;

class ModelLoader
{
    private:
        map < string, Bone* > bones; 
        vector < Mesh* > meshes; 
        static map < string, Texture > textures_loaded; 
        string directory; 
        Skeleton *skeleton;

        Importer import; 
        const aiScene* scene; 
        
        map < string, aiNode* > nodes; 
        vector < map < string, AnimationData* > > animations; 

        void processNode(aiNode *node); 
        void processNodeAnim(); 
        void processBone(); 
        void processMeshes(aiNode *node); 
        Mesh* processMesh(aiMesh *mesh); 

        vector < Texture > loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName); 
        unsigned int textureFromFile(string filename); 

        Bone* findBone(string name) const; 
        int findBoneId(string name) const; 
        aiNode* findAiNode(string name) const; 
        vector < AnimationData* > findAiNodeAnims(string name) const; 

    public:
        ModelLoader();

        void loadModel(string path); 

        void getModelData(Skeleton *&skeleton, vector < Mesh* > &meshes) const; 

        void clear();

        ~ModelLoader();
};
