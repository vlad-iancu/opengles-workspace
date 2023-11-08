#include <renderer.hpp>
#include <exception.hpp>

#include <memory>
#include <optional>
#include <cassert>
#include <array>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace opengles_workspace
{
	GLFWRenderer::GLFWRenderer(std::shared_ptr<Context> context)
		: mContext(std::move(context))
	{
	}

	void GLFWRenderer::render() {
		// GL code begin
		
		// GL code goes here

		// GL code end
		glfwSwapBuffers(window());
	}

	bool GLFWRenderer::poll() {
		if (glfwWindowShouldClose(window())) {
			return false;
		}
		return true;
	}
}