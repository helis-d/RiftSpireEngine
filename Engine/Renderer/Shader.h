#pragma once

#include "../Core/Types.h"
#include <glm/glm.hpp>
#include <string>

namespace RiftSpire
{
    class Shader
    {
    public:
        Shader(const std::string& vertexSource, const std::string& fragmentSource);
        Shader(const std::string& filepath); // Load from file
        ~Shader();

        // Non-copyable
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        void Bind() const;
        void Unbind() const;

        // Uniform setters
        void SetInt(const std::string& name, int value);
        void SetIntArray(const std::string& name, int* values, u32 count);
        void SetFloat(const std::string& name, float value);
        void SetFloat2(const std::string& name, const glm::vec2& value);
        void SetFloat3(const std::string& name, const glm::vec3& value);
        void SetFloat4(const std::string& name, const glm::vec4& value);
        void SetMat3(const std::string& name, const glm::mat3& value);
        void SetMat4(const std::string& name, const glm::mat4& value);

        u32 GetRendererID() const { return m_RendererID; }

        static Ref<Shader> Create(const std::string& vertexSource, const std::string& fragmentSource);
        static Ref<Shader> CreateFromFile(const std::string& filepath);

    private:
        void Compile(const std::string& vertexSource, const std::string& fragmentSource);
        int GetUniformLocation(const std::string& name) const;

    private:
        u32 m_RendererID = 0;
        mutable std::unordered_map<std::string, int> m_UniformLocationCache;
    };
}
