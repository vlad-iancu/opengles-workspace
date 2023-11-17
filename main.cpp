#include <stdio.h>
#include <iostream>
#include <cassert>
#include "glfw_application.hpp"
#include "SnakeGameState.hpp"
using namespace opengles_workspace;

int main()
{
    GlfwApplication app(640, 640);
    return app.run();
}