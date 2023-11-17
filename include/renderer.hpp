#pragma once
#include <memory>
#include <optional>
#include <context.hpp>
#include <polled_object.hpp>
#include "shader.hpp"
#include "SnakeGameState.hpp"
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct Vertex
{
	float x, y, z;
	float r, g, b;
	float u, v;
};

namespace opengles_workspace
{
class GLFWRenderer : public PolledObject
	{
	public:
		GLFWRenderer(
			std::shared_ptr<Context> context,
			std::shared_ptr<Shader> pShader
		);

		~GLFWRenderer() = default;

		void render();

		bool poll() override;
	private:
		std::shared_ptr<Shader> pShader;
		unsigned int VAO;
		unsigned int VBO;
		unsigned int EBO;
		SnakeGameState mGameState;
		unsigned int mTexture;
		std::shared_ptr<Context> mContext;
		GLFWwindow* window() const { return static_cast<GLFWwindow*>(mContext->window()); }
	};
}