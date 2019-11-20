#pragma once

//native
#include <stdexcept>
#include <thread>

//openGL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Window : public GLFWEvents
{
    private:
        bool keysPressed[512];
        bool buttonsPressed[64];
        
        bool keysPressedOnce[512];
        bool buttonsPressedOnce[64];

        GLFWwindow* window;

        bool cursor;

        int width, height;
        bool resized;

        int renderWidth, renderHeight;

        vec2 mousePosition;
        bool mouseMoved;

        Shader* renderShader;
        RenderQuad* renderQuad;
      
        void close_window() override;
        void reset_window_size(double width, double height) override;
        void mark_pressed(int key) override;
        void mark_released(int key) override;
        void mark_mouse_pressed(int button) override;
        void mark_mouse_released(int button) override;
        void mouse_moved(double posx, double posy) override;

        void clearEventsData();

    public:
        Window();
        
        /**********/
        
        bool isCursor() const;

        bool isOpen() const;

        bool isKeyPressed(int key) const;
        bool isKeyPressedOnce(int key) const;
        bool isButtonPressed(int button) const;
        bool isButtonPressedOnce(int button) const;

        bool isResized() const;
        vec2 getSize() const;
        vec2 getRenderSize() const;
        
        bool isMouseMoved() const;
        vec2 getMousePosition() const;

        float getTime() const;

        /**********/

        void makeCurrentContext();
        void detachCurrentContext();

        void pollEvents();

        void hideCursor();
        void showCursor();

        void render(GLuint finalTexture);
        void close();

        ~Window();
};
