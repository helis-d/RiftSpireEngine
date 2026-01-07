#include "Input.h"
#include <GLFW/glfw3.h>

namespace RiftSpire
{
    GLFWwindow* Input::s_Window = nullptr;
    glm::vec2 Input::s_LastMousePosition = { 0.0f, 0.0f };
    glm::vec2 Input::s_MouseDelta = { 0.0f, 0.0f };
    float Input::s_ScrollDelta = 0.0f;

    static float s_ScrollAccumulator = 0.0f;

    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        s_ScrollAccumulator += static_cast<float>(yoffset);
    }

    void Input::Init(GLFWwindow* window)
    {
        s_Window = window;
        
        double x, y;
        glfwGetCursorPos(s_Window, &x, &y);
        s_LastMousePosition = { static_cast<float>(x), static_cast<float>(y) };

        glfwSetScrollCallback(s_Window, ScrollCallback);
    }

    void Input::Update()
    {
        // Update mouse delta
        glm::vec2 currentPos = GetMousePosition();
        s_MouseDelta = currentPos - s_LastMousePosition;
        s_LastMousePosition = currentPos;

        // Update scroll delta
        s_ScrollDelta = s_ScrollAccumulator;
        s_ScrollAccumulator = 0.0f;
    }

    bool Input::IsKeyPressed(KeyCode key)
    {
        int state = glfwGetKey(s_Window, static_cast<int>(key));
        return state == GLFW_PRESS;
    }

    bool Input::IsKeyReleased(KeyCode key)
    {
        int state = glfwGetKey(s_Window, static_cast<int>(key));
        return state == GLFW_RELEASE;
    }

    bool Input::IsKeyHeld(KeyCode key)
    {
        int state = glfwGetKey(s_Window, static_cast<int>(key));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressed(MouseButton button)
    {
        int state = glfwGetMouseButton(s_Window, static_cast<int>(button));
        return state == GLFW_PRESS;
    }

    bool Input::IsMouseButtonReleased(MouseButton button)
    {
        int state = glfwGetMouseButton(s_Window, static_cast<int>(button));
        return state == GLFW_RELEASE;
    }

    glm::vec2 Input::GetMousePosition()
    {
        double x, y;
        glfwGetCursorPos(s_Window, &x, &y);
        return { static_cast<float>(x), static_cast<float>(y) };
    }

    float Input::GetMouseX()
    {
        return GetMousePosition().x;
    }

    float Input::GetMouseY()
    {
        return GetMousePosition().y;
    }

    glm::vec2 Input::GetMouseDelta()
    {
        return s_MouseDelta;
    }

    float Input::GetScrollDelta()
    {
        return s_ScrollDelta;
    }
}
