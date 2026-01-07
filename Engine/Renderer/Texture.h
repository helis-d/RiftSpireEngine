#pragma once

#include "../Core/Types.h"
#include <string>

namespace RiftSpire
{
    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual u32 GetWidth() const = 0;
        virtual u32 GetHeight() const = 0;
        virtual u32 GetRendererID() const = 0;

        virtual void SetData(void* data, u32 size) = 0;
        virtual void Bind(u32 slot = 0) const = 0;
        virtual bool IsLoaded() const = 0;

        virtual bool operator==(const Texture& other) const = 0;
    };

    class Texture2D : public Texture
    {
    public:
        Texture2D(u32 width, u32 height);
        Texture2D(const std::string& path);
        ~Texture2D();

        u32 GetWidth() const override { return m_Width; }
        u32 GetHeight() const override { return m_Height; }
        u32 GetRendererID() const override { return m_RendererID; }

        void SetData(void* data, u32 size) override;
        void Bind(u32 slot = 0) const override;
        bool IsLoaded() const override { return m_IsLoaded; }

        bool operator==(const Texture& other) const override
        {
            return m_RendererID == other.GetRendererID();
        }

        static Ref<Texture2D> Create(u32 width, u32 height);
        static Ref<Texture2D> Create(const std::string& path);

    private:
        std::string m_Path;
        u32 m_Width = 0;
        u32 m_Height = 0;
        u32 m_RendererID = 0;
        u32 m_InternalFormat = 0;
        u32 m_DataFormat = 0;
        bool m_IsLoaded = false;
    };
}
