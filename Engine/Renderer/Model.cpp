#include "Model.h"
#include "ModelLoader.h"

namespace RiftSpire
{
    Ref<Model> Model::Load(const std::string& path)
    {
        return ModelLoader::LoadModel(path);
    }

    Ref<Model> Model::LoadFromMemory(const void* data, size_t size, const std::string& hint)
    {
        return ModelLoader::LoadModelFromMemory(data, size, hint);
    }

    void Model::Draw(const Ref<Shader>& shader)
    {
        shader->SetMat4("u_Model", m_Transform);
        
        for (auto& mesh : m_Meshes)
        {
            mesh.Draw(shader);
        }
    }

    void Model::DrawInstanced(const Ref<Shader>& shader, u32 instanceCount)
    {
        shader->SetMat4("u_Model", m_Transform);
        
        for (auto& mesh : m_Meshes)
        {
            mesh.DrawInstanced(shader, instanceCount);
        }
    }
}
