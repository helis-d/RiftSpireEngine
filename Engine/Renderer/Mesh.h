#pragma once

#include "../Core/Types.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace RiftSpire
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

    struct MeshTexture
    {
        Ref<Texture2D> Texture;
        std::string Type;    // "diffuse", "specular", "normal", etc.
        std::string Path;    // For comparison
    };

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex>& vertices, 
             const std::vector<u32>& indices,
             const std::vector<MeshTexture>& textures = {});
        ~Mesh() = default;

        void Draw(const Ref<Shader>& shader);
        void DrawInstanced(const Ref<Shader>& shader, u32 instanceCount);

        // Getters
        const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
        const std::vector<u32>& GetIndices() const { return m_Indices; }
        const std::vector<MeshTexture>& GetTextures() const { return m_Textures; }

    private:
        void SetupMesh();

    private:
        std::vector<Vertex> m_Vertices;
        std::vector<u32> m_Indices;
        std::vector<MeshTexture> m_Textures;

        Ref<VertexArray> m_VAO;
        Ref<VertexBuffer> m_VBO;
        Ref<IndexBuffer> m_IBO;
    };
}
