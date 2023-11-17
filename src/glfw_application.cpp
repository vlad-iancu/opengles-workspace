#include "glfw_application.hpp"
#include "exception.hpp"
#include "context.hpp"
#include "input.hpp"
#include "main_loop.hpp"
#include "renderer.hpp"

#include <memory>
#include <iostream>
#include <cassert>

#define GLFW_WINDOW(ptr) reinterpret_cast<GLFWwindow*>(ptr)

namespace opengles_workspace
{
typedef void(*GLFWwindowDeleter)(GLFWwindow*);

void error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW error: %s\n", description);
}

void destroyGlfwWindow(GLFWwindow* window) {
	glfwDestroyWindow(window);
}

GlfwApplication::GlfwApplication(size_t width, size_t height)
	: mWidth(width)
	, mHeight(height)
{
	auto status = glfwInit();
	std::cout << status << std::endl;
	if(!status) {
        throw Exception("Failed to initialize GLFW");
    }
    glfwSetErrorCallback(error_callback);
}

GlfwApplication::~GlfwApplication() {
	glfwTerminate();
}

int GlfwApplication::run() {
	auto pWindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(glfwCreateWindow(mWidth, mHeight, "OpenGL ES workspace", nullptr, nullptr), destroyGlfwWindow);
	if(!pWindow) {
		throw Exception("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(pWindow.get());
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	MainLoop loop;
	auto ctx = std::make_shared<Context>(pWindow.get());
	std::shared_ptr<Input> pInput(Input::create(ctx));
	std::shared_ptr<Shader> pShader = std::make_shared<Shader>("vertexShader.vert", "fragmentShader.frag");
	std::shared_ptr<SnakeGameState> pGameState = std::make_shared<SnakeGameState>(BOARD_WIDTH, BOARD_HEIGHT, pShader, ctx);
	pGameState->init();
	pGameState->setSnakeDirection(0, 1);
	pGameState->setSnakePosition(0, 2);
	pGameState->setSnakeTail(0, 0);
	pGameState->setBoard(0, 0, LEFT_MASK);
	pGameState->setBoard(0, 1, LEFT_MASK);
	pGameState->setBoard(0, 2, LEFT_MASK);
	std::shared_ptr<GLFWRenderer> pRenderer = std::make_shared<GLFWRenderer>(ctx, pShader);
	pInput->registerKeyCallback([&](Key key, KeyMode keyMode) {
			if (key == Key::ESCAPE && keyMode == KeyMode::PRESS) {
				glfwSetWindowShouldClose(pWindow.get(), GLFW_TRUE);
				return false;
			}
			if (key == Key::LEFT && keyMode == KeyMode::PRESS) {
				if(pGameState->getSnakeDirectionY() != 1)
				pGameState->setSnakeDirection(0, -1);
				
				return false;
			}
			if (key == Key::DOWN && keyMode == KeyMode::PRESS) {
				if(pGameState->getSnakeDirectionX() != -1)
				pGameState->setSnakeDirection(1, 0);
				return false;
			}
			if (key == Key::RIGHT && keyMode == KeyMode::PRESS) {
				if(pGameState->getSnakeDirectionY() != -1)
				pGameState->setSnakeDirection(0, 1);
				return false;
			}
			if (key == Key::UP && keyMode == KeyMode::PRESS) {
				if(pGameState->getSnakeDirectionX() != 1)
				pGameState->setSnakeDirection(-1, 0);
				return false;
			}
			return true;
		});

	loop.addPolledObject(pInput);
	loop.addPolledObject(pRenderer);
	loop.addPolledObject(pGameState);
	pRenderer->render();
	loop.run();
	return 0;
}
}