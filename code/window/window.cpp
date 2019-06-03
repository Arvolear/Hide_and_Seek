#include "../global/convert.hpp"

#include "../shader/shader.hpp"

#include "glfwevents.hpp"
#include "renderquad.hpp"
#include "window.hpp"

Window::Window() : GLFWEvents()
{
    cursor = true;

    width = 1920;
    height = 1080;
    resized = false;

    renderWidth = 1920;
    renderHeight = 1080;

    mousePosition = vec2(0);
    mouseMoved = false;

    if (!glfwInit())
    {
        throw runtime_error("ERROR::Failed to initialize glfw");
    }

    //glfwWindowHint(GLFW_SAMPLES, 4);

    /* 30 FPS */
    glfwWindowHint(GLFW_REFRESH_RATE, 30);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Hide&Seek", NULL, NULL);

    if (!window)
    {
        glfwTerminate();

        throw runtime_error("ERROR::Failed to initialize window");
    }

    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window); // make current widow active
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   
    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_move_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    glewExperimental = GL_TRUE; // new version of glew
    if (glewInit() != GLEW_OK) // trying to init glew
    {
        throw runtime_error("ERROR::Failed to initialize GLEW");
    }
    
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_FRAMEBUFFER_SRGB); 
        
    glCullFace(GL_BACK);

    /* LOAD SHADERS AT FIRST */
    renderShader = new Shader();
    renderShader->loadShaders(path("code/shader/vertexRenderShader.glsl"), path("code/shader/fragmentRenderShader.glsl")); //loading shaders

    renderQuad = new RenderQuad();
    renderQuad->init();
}

/******************/

void Window::close_window()
{
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void Window::reset_window_size(double width, double height)
{
    resized = true;
    this->width = width;
    this->height = height;
}

void Window::mark_pressed(int key)
{
    keysPressed[key] = true;
    keysPressedOnce[key] = true;
}

void Window::mark_released(int key)
{
    keysPressed[key] = false;
    keysPressedOnce[key] = false;
}

void Window::mark_mouse_pressed(int button)
{
    buttonsPressed[button] = true;
    buttonsPressedOnce[button] = true;
}

void Window::mark_mouse_released(int button)
{
    buttonsPressed[button] = false;
    buttonsPressedOnce[button] = false;
}

void Window::mouse_moved(double posx, double posy)
{
    mouseMoved = true;
    mousePosition = vec2(posx, posy);
}

/******************/

bool Window::isCursor() const
{
    return cursor;
}

bool Window::isOpen() const
{
    return !glfwWindowShouldClose(window);
}

bool Window::isKeyPressed(int key) const
{
    return keysPressed[key];
}

bool Window::isKeyPressedOnce(int key) const
{
    return keysPressedOnce[key];
}

bool Window::isButtonPressed(int button) const
{
    return buttonsPressed[button];
}

bool Window::isButtonPressedOnce(int button) const
{
    return buttonsPressedOnce[button];
}

bool Window::isResized() const
{
    return resized;
}

vec2 Window::getSize() const
{
    return vec2(width, height); 
}
        
vec2 Window::getRenderSize() const
{
    return vec2(renderWidth, renderHeight);
}

bool Window::isMouseMoved() const
{
    return mouseMoved;
}

vec2 Window::getMousePosition() const
{
    return mousePosition;
}
        
float Window::getTime() const
{
    return glfwGetTime();
}

/******************/

void Window::hideCursor()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    cursor = false;
}

void Window::showCursor()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    cursor = true;
}

void Window::pollEvents()
{
    glfwPollEvents();
}
        
void Window::clearEventsData()
{
    resized = false;
    mouseMoved = false;

    memset(keysPressedOnce, 0, sizeof(bool) * 512);
    memset(buttonsPressedOnce, 0, sizeof(bool) * 64);
}

void Window::render(GLuint finalTexture)
{
    clearEventsData();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height); // set visible

    glClearColor(0.0f, 0.2f, 0.0f, 1.0f); // black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderShader->use();

    renderShader->setInt("finalTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, finalTexture);

    renderQuad->render(renderShader);

    glfwSwapBuffers(window); 
}

void Window::close()
{
    close_window();
}

Window::~Window()
{
    delete renderShader;
    delete renderQuad;

    glfwDestroyWindow(window);
    glfwTerminate();
}
