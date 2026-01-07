#include "Mesh.h"
#include <glad/gl.h>

namespace RiftSpire
{
    Mesh::Mesh(const std::vector<Vertex>& vertices,
               const std::vector<u32>& indices,
               const std::vector<MeshTexture>& textures)
        : m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
    {
        SetupMesh();
    }

    void Mesh::SetupMesh()
    {
        m_VAO = CreateRef<VertexArray>();

        // Create vertex buffer
        m_VBO = CreateRef<VertexBuffer>(reinterpret_cast<const float*>(m_Vertices.data()),
                                        static_cast<u32>(m_Vertices.size() * sizeof(Vertex)));

        // Define vertex layout
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" },
            { ShaderDataType::Float2, "a_TexCoords" },
            { ShaderDataType::Float3, "a_Tangent" },
            { ShaderDataType::Float3, "a_Bitangent" }
        };
        m_VBO->SetLayout(layout);
        m_VAO->AddVertexBuffer(m_VBO);

        // Create index buffer
        m_IBO = CreateRef<IndexBuffer>(m_Indices.data(), static_cast<u32>(m_Indices.size()));
        m_VAO->SetIndexBuffer(m_IBO);
    }

    void Mesh::Draw(const Ref<Shader>& shader)
    {
        // Bind textures
        u32 diffuseNr = 1;
        u32 specularNr = 1;
        u32 normalNr = 1;
        u32 heightNr = 1;

        for (u32 i = 0; i < m_Textures.size(); i++)
        {
            std::string number;
            std::string name = m_Textures[i].Type;

            if (name == "diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "specular")
                number = std::to_string(specularNr++);
            else if (name == "normal")
                number = std::to_string(normalNr++);
            else if (name == "height")
                number = std::to_string(heightNr++);

            shader->SetInt("u_Material." + name + number, i);
            m_Textures[i].Texture->Bind(i);
        }

        // Draw mesh
        m_VAO->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
        m_VAO->Unbind();

        // Reset active texture
        glActiveTexture(GL_TEXTURE0);
    }

    void Mesh::DrawInstanced(const Ref<Shader>& shader, u32 instanceCount)
    {
        // Bind textures (same as Draw)
        for (u32 i = 0; i < m_Textures.size(); i++)
        {
            m_Textures[i].Texture->Bind(i);
        }

        m_VAO->Bind();
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), 
                                GL_UNSIGNED_INT, nullptr, instanceCount);
        m_VAO->Unbind();
    }
}
