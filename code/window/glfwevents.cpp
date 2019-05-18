#include "glfwevents.hpp"

void GLFWEvents::window_size_callback(GLFWwindow* window, int width, int height)
{
    static_cast < GLFWEvents* > (glfwGetWindowUserPointer(window))->reset_window_size(width, height);
}

void GLFWEvents::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        static_cast < GLFWEvents* > (glfwGetWindowUserPointer(window))->mark_pressed(key);
    }

    if (action == GLFW_RELEASE)
    {
        static_cast < GLFWEvents* > (glfwGetWindowUserPointer(window))->mark_released(key);
    }
}

void GLFWEvents::mouse_move_callback(GLFWwindow* window, double posx, double posy)
{
    static_cast < GLFWEvents* > (glfwGetWindowUserPointer(window))->mouse_moved(posx, posy);
}

void GLFWEvents::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        static_cast < GLFWEvents* > (glfwGetWindowUserPointer(window))->mark_mouse_pressed(button);
    }

    if (action == GLFW_RELEASE)
    {
        static_cast < GLFWEvents* > (glfwGetWindowUserPointer(window))->mark_mouse_released(button);
    }
}
