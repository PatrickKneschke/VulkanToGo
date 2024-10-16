
#include "input_handler.h"

#include <iostream>

namespace vktg
{


    vktg::InputLayer::InputLayer() : 
        keyCallback{ [](const bool*){} },
        mouseButtonCallback{ [](const bool*){} },
        cursorPosCallback{ [](double, double, double, double){} },
        next{nullptr} {} 


    void InputHandler::KeyListener(GLFWwindow *window, int key, int scancode, int action, int mods) {

        auto inputHandler = (InputHandler*)glfwGetWindowUserPointer( window);
        inputHandler->mKeyPress[key] = action != GLFW_RELEASE;
    }

    
    void InputHandler::MouseListener(GLFWwindow *window, int button, int action, int mods) {

        auto inputHandler = (InputHandler*)glfwGetWindowUserPointer( window);
        inputHandler->mMouseButtonPress[button] = action != GLFW_RELEASE;
    }

    
    void InputHandler::CursorPosListener(GLFWwindow *window, double xpos, double ypos) {

        auto inputHandler = (InputHandler*)glfwGetWindowUserPointer( window);

        inputHandler->mDeltaX = xpos - inputHandler->mMouseX;
        inputHandler->mDeltaY = ypos - inputHandler->mMouseY;
        inputHandler->mMouseX = xpos;
        inputHandler->mMouseY = ypos;
    }


    InputHandler::InputHandler(GLFWwindow *window) : pWindow{window} {

        glfwSetWindowUserPointer( window, this);

        glfwSetKeyCallback( pWindow, KeyListener);
        glfwSetMouseButtonCallback( pWindow, MouseListener);
        glfwSetCursorPosCallback( pWindow, CursorPosListener);

        glfwSetInputMode( window, GLFW_STICKY_KEYS, GLFW_TRUE);
        glfwSetInputMode( pWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }


    void InputHandler::CaptureCursor( bool capture) {

        if (capture)
        {
            glfwSetInputMode( pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else 
        {
            glfwSetInputMode( pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }


    bool InputHandler::KeyPressed(int key) const
    {

        return mKeyPress[key];
    }

    bool InputHandler::MouseButtonPressed(int button) const {

        return mMouseButtonPress[button];
    }

    
    std::pair<double, double> InputHandler::CursorPos() const
    {
        return {mMouseX, mMouseY};
    }


    std::pair<double, double> InputHandler::CursorDelta() const {

        return {mDeltaX, mDeltaY};
    }


    void InputHandler::PushLayer(InputLayer *pNewLayer)
    {

        pNewLayer->next = pTopLayer;
        pTopLayer = pNewLayer;
    }


    void InputHandler::PopLayer() {

        if (pTopLayer != nullptr)
        {
            auto temp = pTopLayer;
            pTopLayer = pTopLayer->next;
            temp->next = nullptr;
        }
    }
    
    
    void InputHandler::Update() {

        mDeltaX = 0.0;
        mDeltaY = 0.0;
        
        glfwPollEvents();
        if (pTopLayer != nullptr)
        {
            pTopLayer->keyCallback( mKeyPress);
            pTopLayer->mouseButtonCallback( mMouseButtonPress);
            pTopLayer->cursorPosCallback( mMouseX, mMouseY, mDeltaX, mDeltaY);
        }
    }


} // namespace vktg
