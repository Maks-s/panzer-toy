#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "shaders.hpp"
#include "errors.hpp"
#include "mesh.hpp"

GLFWwindow* initGL();

int main() {
	GLFWwindow* window = initGL();
	if (window == nullptr)
		return -1;

	glViewport(0, 0, 500, 500);

	float vertices[] = {
		-1.0f, -0.5f, 0.0f,
		 0.0f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
	};

	float vertices2[] = {
		 0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		 1.0f,  0.0f, 0.0f,
	};

	mesh triangle(vertices, sizeof(vertices));
	triangle.vertexAttrib(0, 3);

	mesh triangle2(vertices2, sizeof(vertices2));
	triangle2.vertexAttrib(0, 3);

	shader baseShader("shaders/vertex.glsl", "shaders/fragment.glsl");
	if (baseShader.fail())
		return -1;

	GLint uniformTime = baseShader.getUniformLocation("time");

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		float time = glfwGetTime();

		baseShader.setUniform(uniformTime, time);
		baseShader.use();

		triangle.bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);

		baseShader.setUniform(uniformTime, time - 0.2f);

		triangle2.bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

/**
	* Initialise GLFW and GL3W
	*
	* @return nullptr if there's an error
**/
GLFWwindow* initGL() {
	if (!glfwInit()) {
		PRINT_ERROR("Error initialising GLFW");
		return nullptr;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// @TODO: Implement resizability
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(500, 500, "Panzer Toy", nullptr, nullptr);
	if (window == nullptr) {
		PRINT_ERROR("Error creating window");
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (gl3wInit()) {
		PRINT_ERROR("Error initialising GL3W");
		glfwTerminate();
		return nullptr;
	}

	return window;
}
