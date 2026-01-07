#pragma once

#include "../Core/Types.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Camera.h"
#include <glm/glm.hpp>

namespace RiftSpire
{
    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();
        static void SetViewport(u32 x, u32 y, u32 width, u32 height);
        static void SetClearColor(const glm::vec4& color);
        static void Clear();
        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();
        static void DrawIndexed(const Ref<VertexArray>& va, u32 count = 0);
        static void EnableBlending();
        static void DisableBlending();

    private:
        struct SceneData { glm::mat4 ViewProjectionMatrix; };
        static Scope<SceneData> s_SceneData;
    };
}
