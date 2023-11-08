#include <stdio.h>
#include <cassert>
#include "glfw_application.hpp"

using namespace opengles_workspace;

int main()
{
    GlfwApplication app(640, 480);
    return app.run();
}