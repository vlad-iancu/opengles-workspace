#pragma once

#include <cstdlib>

namespace opengles_workspace
{
    class GlfwApplication
    {
    public:
        GlfwApplication(size_t width, size_t height);
        ~GlfwApplication();
        int run();
    private:
        size_t mWidth;
        size_t mHeight;
    };
}