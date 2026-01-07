#include "ModelLoader.h"
#include "../Core/Logger.h"

#include <filesystem>

namespace RiftSpire
{
    Ref<Model> ModelLoader::LoadModel(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, DefaultImportFlags);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            RS_ERROR("Assimp Error: {}", importer.GetErrorString());
            return nullptr;
        }

        Ref<Model> model = CreateRef<Model>();
        model->m_FilePath = path;
        model->m_Directory = std::filesystem::path(path).parent_path().string();

        RS_INFO("Loading model: {}", path);
        RS_INFO("  Meshes: {}, Materials: {}", scene->mNumMeshes, scene->mNumMaterials);

        ProcessNode(scene->mRootNode, scene, model.get());

        RS_INFO("Model loaded successfully with {} meshes", model->m_Meshes.size());
        return model;
    }

    Ref<Model> ModelLoader::LoadModelFromMemory(const void* data, size_t size, const std::string& hint)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(data, size, DefaultImportFlags, hint.c_str());

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            RS_ERROR("Assimp Error (memory): {}", importer.GetErrorString());
            return nullptr;
        }

        Ref<Model> model = CreateRef<Model>();
        model->m_FilePath = "memory";
        model->m_Directory = "";

        ProcessNode(scene->mRootNode, scene, model.get());
        return model;
    }

    void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, Model* model)
    {
        // Process all meshes in this node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            model->m_Meshes.push_back(ProcessMesh(mesh, scene, model));
        }

        // Recursively process child nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, model);
        }
    }

    Mesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, Model* model)
    {
        std::vector<Vertex> vertices;
        std::vector<u32> indices;
        std::vector<MeshTexture> textures;

        // Process vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            // Position
            vertex.Position = glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );

            // Normals
            if (mesh->HasNormals())
            {
                vertex.Normal = glm::vec3(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
                );
            }
            else
            {
                vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            // Texture coordinates
            if (mesh->mTextureCoords[0])
            {
                vertex.TexCoords = glm::vec2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                );

                // Tangent
                if (mesh->HasTangentsAndBitangents())
                {
                    vertex.Tangent = glm::vec3(
                        mesh->mTangents[i].x,
                        mesh->mTangents[i].y,
                        mesh->mTangents[i].z
                    );
                    vertex.Bitangent = glm::vec3(
                        mesh->mBitangents[i].x,
                        mesh->mBitangents[i].y,
                        mesh->mBitangents[i].z
                    );
                }
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // Process indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Process material
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // Diffuse maps
            auto diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse", model);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // Specular maps
            auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "specular", model);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            // Normal maps
            auto normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, "normal", model);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            // Height maps
            auto heightMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "height", model);
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        return Mesh(vertices, indices, textures);
    }

    std::vector<MeshTexture> ModelLoader::LoadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                                                const std::string& typeName, Model* model)
    {
        std::vector<MeshTexture> textures;

        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            // Check if texture was already loaded
            bool skip = false;
            for (auto& loadedTex : model->m_LoadedTextures)
            {
                if (loadedTex.Path == str.C_Str())
                {
                    textures.push_back(loadedTex);
                    skip = true;
                    break;
                }
            }

            if (!skip)
            {
                MeshTexture texture;
                std::string texturePath = model->m_Directory + "/" + str.C_Str();
                
                texture.Texture = Texture2D::Create(texturePath);
                texture.Type = typeName;
                texture.Path = str.C_Str();

                textures.push_back(texture);
                model->m_LoadedTextures.push_back(texture);
            }
        }

        return textures;
    }
}
