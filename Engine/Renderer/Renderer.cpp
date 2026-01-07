#include "Renderer.h"
#include "../Core/Logger.h"
#include <glad/gl.h>

namespace RiftSpire
{
    Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

    void Renderer::Init()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        RS_ENGINE_INFO("Renderer initialized");
    }

    void Renderer::Shutdown() {}

    void Renderer::SetViewport(u32 x, u32 y, u32 width, u32 height)
    {
        glViewport(x, y, width, height);
    }

    void Renderer::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void Renderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::BeginScene(const OrthographicCamera& camera)
    {
        s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene() {}

    void Renderer::DrawIndexed(const Ref<VertexArray>& va, u32 count)
    {
        va->Bind();
        u32 c = count ? count : va->GetIndexBuffer()->GetCount();
        glDrawElements(GL_TRIANGLES, c, GL_UNSIGNED_INT, nullptr);
    }

    void Renderer::EnableBlending()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Renderer::DisableBlending()
    {
        glDisable(GL_BLEND);
    }
}
