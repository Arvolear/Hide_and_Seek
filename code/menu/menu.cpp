#include "../global/convert.hpp"
#include "../window/glfwevents.hpp"
#include "../global/shader.hpp"
#include "../window/framebuffer.hpp"
#include "../window/renderquad.hpp"
#include "../window/window.hpp"
#include "menu.hpp"

Menu::Menu()
{
    window = new Window();
    menuShader = new Shader();
    menuBuffer = new FrameBuffer();
}

void Menu::menuLoop()
{
    while (window->isOpen())
    {
        if (window->isKeyPressed(GLFW_KEY_ESCAPE))
        {
            window->close();
        }

        window->render(0);
    }
}

Menu::~Menu()
{
    delete window;

    delete menuShader;
    delete menuBuffer;

    //delete setting
    //delete game
}
