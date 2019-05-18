#include "../global/convert.hpp"
#include "glfwevents.hpp"
#include "../global/shader.hpp"
#include "framebuffer.hpp"
#include "renderquad.hpp"
#include "window.hpp"

Window::Window() : GLFWEvents()
{
    width = 1920;
    height = 1080;
    resized = false;

    renderWidth = 1920;
    renderHeight = 1080;

    mousePosition = vec2(0);
    mouseMoved = false;

    renderShader = new Shader();
    renderQuad = new RenderQuad();

    if (!glfwInit())
    {
        cout << "Failed to initialize glfw" << endl;
        return;
    }

    //glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Hide&Seek", NULL, NULL);

    if (!window)
    {
        glfwTerminate();

        cout << "Failed to initialize window" << endl;
        return;
    }

    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window); // make current widow active
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_move_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    glewExperimental = GL_TRUE; // new version of glew
    if (glewInit() != GLEW_OK) // trying to init glew
    {
        cout << "Failed to initialize GLEW" << endl;
        return;
    }
    
    renderShader->loadShaders(path("./code/window/vertexRenderShader.glsl"), \
                              path("./code/window/fragmentRenderShader.glsl")); //loading shaders

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_FRAMEBUFFER_SRGB); 
}

/******************/

void Window::reset_window_size(double width, double height)
{
    resized = true;
    this->width = width;
    this->height = height;
}

void Window::mark_pressed(int key)
{
    keysPressed[key] = true;
}

void Window::mark_released(int key)
{
    keysPressed[key] = false;
}

void Window::mark_mouse_pressed(int button)
{
    buttonsPressed[button] = true;
}

void Window::mark_mouse_released(int button)
{
    buttonsPressed[button] = false;
}

void Window::mouse_moved(double posx, double posy)
{
    mouseMoved = true;
    mousePosition = vec2(posx, posy);
}

/******************/
        
bool Window::isOpen() const
{
    return !glfwWindowShouldClose(window);
}

bool Window::isKeyPressed(int key) const
{
    return keysPressed[key];
}

bool Window::isButtonPressed(int button) const
{
    return buttonsPressed[button];
}

bool Window::isResized() const
{
    return resized;
}

vec2 Window::getSize() const
{
    return vec2(width, height); 
}

bool Window::isMouseMoved() const
{
    return mouseMoved;
}

vec2 Window::getMousePosition() const
{
    return mousePosition;
}

/******************/

void Window::render(GLuint finalTexture)
{
    glfwPollEvents();

    resized = false;
    mouseMoved = false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, width, height); //set visible

    glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderShader->use();

    glUniform1i(glGetUniformLocation(renderShader->ID, "finalTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, finalTexture);

    renderQuad->draw(renderShader);

    glfwSwapBuffers(window); 
}

void Window::close()
{
    glfwSetWindowShouldClose(window, GL_TRUE);
}

Window::~Window()
{
    delete renderShader;
    delete renderQuad;

    glfwDestroyWindow(window);
    glfwTerminate();
}
