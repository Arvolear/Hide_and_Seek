#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GLFWEvents
{
    protected:
        static void  window_close_callback(GLFWwindow* window);

        static void window_size_callback(GLFWwindow* window, int width, int height);

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

        static void mouse_move_callback(GLFWwindow* window, double posx, double posy);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

        GLFWEvents(){}
        virtual void close_window() = 0;
        virtual void reset_window_size(double width, double height) = 0; 
        virtual void mark_pressed(int key) = 0;
        virtual void mark_released(int key) = 0;
        virtual void mark_mouse_pressed(int button) = 0;
        virtual void mark_mouse_released(int button) = 0;
        virtual void mouse_moved(double posx, double posy) = 0;
        
        virtual ~GLFWEvents(){}
};
