#include <renderer.hpp>
#include <exception.hpp>

#include <memory>
#include <optional>
#include <cassert>
#include <array>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace opengles_workspace
{
	GLFWRenderer::GLFWRenderer(std::shared_ptr<Context> context)
		: mContext(std::move(context)),
		mShader("vertexShader.vert", "fragmentShader.frag")
	{
		//Create four vertices of a rectangle
		std::array<Vertex, 4> vertices = {
			
			Vertex{-0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
			Vertex{0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f},
			Vertex{0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 4.0f, 4.0f},
			Vertex{-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 4.0f}
		};

		//Create indices
		std::array<unsigned int, 6> indices = {
			0, 1, 2,
			0, 2, 3
		};

		//Create a vertex array object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		//Create a vertex buffer object
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices.data(), GL_STATIC_DRAW);

		//Create an element buffer object
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices.data(), GL_STATIC_DRAW);

		//Set vertex attribute pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
		glEnableVertexAttribArray(2);

		//Unbind the vertex array object
		glBindVertexArray(0);

		//Unbind the vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Unbind the element buffer object
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// load image, create texture and generate mipmaps
		int width, height, nrChannels;
		// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
		//Flip on load
		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load("Chessboard.png", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
		mTexture = texture;
	}

	void GLFWRenderer::render() {
		//Render the triangle
		mShader.use();
		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glfwSwapBuffers(window());
	}

	bool GLFWRenderer::poll() {
		if (glfwWindowShouldClose(window())) {
			return false;
		}
		return true;
	}
}