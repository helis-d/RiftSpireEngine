#pragma once

#include "Types.h"
#include <glm/glm.hpp>

struct GLFWwindow;

namespace RiftSpire
{
    // Key codes (matching GLFW)
    enum class KeyCode : u16
    {
        // Letters
        A = 65, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        // Numbers
        D0 = 48, D1, D2, D3, D4, D5, D6, D7, D8, D9,

        // Function keys
        F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        // Arrow keys
        Right = 262, Left, Down, Up,

        // Special keys
        Space = 32,
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        LeftShift = 340,
        RightShift = 344,
        LeftControl = 341,
        RightControl = 345,
        LeftAlt = 342,
        RightAlt = 346
    };

    // Mouse buttons
    enum class MouseButton : u8
    {
        Left = 0,
        Right = 1,
        Middle = 2,
        Button4 = 3,
        Button5 = 4
    };

    class Input
    {
    public:
        static void Init(GLFWwindow* window);

        // Keyboard
        static bool IsKeyPressed(KeyCode key);
        static bool IsKeyReleased(KeyCode key);
        static bool IsKeyHeld(KeyCode key);

        // Mouse
        static bool IsMouseButtonPressed(MouseButton button);
        static bool IsMouseButtonReleased(MouseButton button);
        static glm::vec2 GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
        static glm::vec2 GetMouseDelta();
        static float GetScrollDelta();

        // Per-frame update (call once per frame before processing input)
        static void Update();

    private:
        static GLFWwindow* s_Window;
        static glm::vec2 s_LastMousePosition;
        static glm::vec2 s_MouseDelta;
        static float s_ScrollDelta;
    };
}
