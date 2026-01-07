#pragma once

#include "../Core/Types.h"
#include <vector>

namespace RiftSpire
{
    // Shader data types
    enum class ShaderDataType
    {
        None = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };

    static u32 ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:   return 4;
            case ShaderDataType::Float2:  return 4 * 2;
            case ShaderDataType::Float3:  return 4 * 3;
            case ShaderDataType::Float4:  return 4 * 4;
            case ShaderDataType::Mat3:    return 4 * 3 * 3;
            case ShaderDataType::Mat4:    return 4 * 4 * 4;
            case ShaderDataType::Int:     return 4;
            case ShaderDataType::Int2:    return 4 * 2;
            case ShaderDataType::Int3:    return 4 * 3;
            case ShaderDataType::Int4:    return 4 * 4;
            case ShaderDataType::Bool:    return 1;
            default: return 0;
        }
    }

    // Buffer element
    struct BufferElement
    {
        std::string Name;
        ShaderDataType Type;
        u32 Size;
        u32 Offset;
        bool Normalized;

        BufferElement() = default;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
        {
        }

        u32 GetComponentCount() const
        {
            switch (Type)
            {
                case ShaderDataType::Float:   return 1;
                case ShaderDataType::Float2:  return 2;
                case ShaderDataType::Float3:  return 3;
                case ShaderDataType::Float4:  return 4;
                case ShaderDataType::Mat3:    return 3 * 3;
                case ShaderDataType::Mat4:    return 4 * 4;
                case ShaderDataType::Int:     return 1;
                case ShaderDataType::Int2:    return 2;
                case ShaderDataType::Int3:    return 3;
                case ShaderDataType::Int4:    return 4;
                case ShaderDataType::Bool:    return 1;
                default: return 0;
            }
        }
    };

    // Buffer layout
    class BufferLayout
    {
    public:
        BufferLayout() = default;

        BufferLayout(std::initializer_list<BufferElement> elements)
            : m_Elements(elements)
        {
            CalculateOffsetsAndStride();
        }

        u32 GetStride() const { return m_Stride; }
        const std::vector<BufferElement>& GetElements() const { return m_Elements; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

    private:
        void CalculateOffsetsAndStride()
        {
            u32 offset = 0;
            m_Stride = 0;
            for (auto& element : m_Elements)
            {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }

    private:
        std::vector<BufferElement> m_Elements;
        u32 m_Stride = 0;
    };

    // Vertex Buffer
    class VertexBuffer
    {
    public:
        VertexBuffer(u32 size);
        VertexBuffer(const float* vertices, u32 size);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;

        void SetData(const void* data, u32 size);

        const BufferLayout& GetLayout() const { return m_Layout; }
        void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

        static Ref<VertexBuffer> Create(u32 size);
        static Ref<VertexBuffer> Create(const float* vertices, u32 size);

    private:
        u32 m_RendererID = 0;
        BufferLayout m_Layout;
    };

    // Index Buffer
    class IndexBuffer
    {
    public:
        IndexBuffer(const u32* indices, u32 count);
        ~IndexBuffer();

        void Bind() const;
        void Unbind() const;

        u32 GetCount() const { return m_Count; }

        static Ref<IndexBuffer> Create(const u32* indices, u32 count);

    private:
        u32 m_RendererID = 0;
        u32 m_Count = 0;
    };
}
