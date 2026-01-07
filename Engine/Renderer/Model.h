#pragma once

#include "../Core/Types.h"
#include "Mesh.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace RiftSpire
{
    class Model
    {
    public:
        Model() = default;
        ~Model() = default;

        // Load model from file
        static Ref<Model> Load(const std::string& path);
        static Ref<Model> LoadFromMemory(const void* data, size_t size, const std::string& hint = "");

        // Draw all meshes
        void Draw(const Ref<Shader>& shader);
        void DrawInstanced(const Ref<Shader>& shader, u32 instanceCount);

        // Getters
        const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }
        const std::string& GetDirectory() const { return m_Directory; }
        const std::string& GetPath() const { return m_FilePath; }

        // Transform for the whole model
        void SetTransform(const glm::mat4& transform) { m_Transform = transform; }
        const glm::mat4& GetTransform() const { return m_Transform; }

    private:
        std::vector<Mesh> m_Meshes;
        std::string m_Directory;
        std::string m_FilePath;
        glm::mat4 m_Transform = glm::mat4(1.0f);

        // Texture cache to avoid reloading
        std::vector<MeshTexture> m_LoadedTextures;

        friend class ModelLoader;
    };
}
