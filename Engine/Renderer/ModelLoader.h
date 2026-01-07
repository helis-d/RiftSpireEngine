#pragma once

#include "../Core/Types.h"
#include "Model.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

namespace RiftSpire
{
    class ModelLoader
    {
    public:
        // Load model from file path
        static Ref<Model> LoadModel(const std::string& path);
        
        // Load model from memory buffer
        static Ref<Model> LoadModelFromMemory(const void* data, size_t size, const std::string& hint = "");

        // Default import flags
        static constexpr unsigned int DefaultImportFlags = 
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace |
            aiProcess_JoinIdenticalVertices |
            aiProcess_OptimizeMeshes;

    private:
        static void ProcessNode(aiNode* node, const aiScene* scene, Model* model);
        static Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, Model* model);
        static std::vector<MeshTexture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, 
                                                              const std::string& typeName, Model* model);
    };
}
