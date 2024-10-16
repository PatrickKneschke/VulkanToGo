#pragma once


#include <GLFW/glfw3.h>

#include <functional>
#include <variant>


namespace vktg
{

    using KeyCallback = std::function<void(const bool*)>;
    using CursorCallback = std::function<void(double,double,double,double)>;

    struct InputLayer {

        KeyCallback keyCallback;
        KeyCallback mouseButtonCallback;
        CursorCallback cursorPosCallback;

        InputLayer *next;

        InputLayer();
    };

    class InputHandler {

        public:

            InputHandler( GLFWwindow* window);

            void CaptureCursor(bool capture = true);

            bool KeyPressed(int key) const;
            bool MouseButtonPressed(int button) const;
            std::pair<double, double> CursorPos() const;
            std::pair<double, double> CursorDelta() const;

            void PushLayer( InputLayer *pLayer);
            void PopLayer();

            void Update();

        private:

            static void KeyListener(GLFWwindow *window, int key, int scancode, int action, int mods);
            static void MouseListener(GLFWwindow *window, int button, int action, int mods);
            static void CursorPosListener(GLFWwindow *window, double xpos, double ypos);


            GLFWwindow* pWindow;
            InputLayer *pTopLayer;

            bool mKeyPress[GLFW_KEY_LAST] = {0};
            bool mMouseButtonPress[GLFW_MOUSE_BUTTON_LAST] = {0};
            double mMouseX, mMouseY;
            double mDeltaX = 0, mDeltaY = 0;
    };

    
} // namespace vktg
