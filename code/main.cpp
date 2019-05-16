#include "global/convert.hpp"
#include "window/glfwevents.hpp"
#include "global/shader.hpp"
#include "window/framebuffer.hpp"
#include "window/renderquad.hpp"
#include "window/window.hpp"
#include "menu/menu.hpp"

int main()
{
    Menu *M = new Menu();

    M->menuLoop();

    delete M;

    return 0;
}
