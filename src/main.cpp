#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "texture.hpp"
#include "shaders.hpp"
#include "errors.hpp"
#include "mesh.hpp"

GLFWwindow* initGL();

int main() {
	GLFWwindow* window = initGL();
	if (window == nullptr)
		return -1;

	float triangleData[] = {
		// vertices          // texture coordinates
		-1.0f, -0.5f, 0.0f,  0.0f, 0.0f,
		 0.0f, -0.5f, 0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,  0.5f, 1.0f,
	};

	float triangleData2[] = {
		// vertices          // texture coordinates
		 0.0f,  0.0f, 0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,  0.5f, 0.5f,
		 1.0f,  0.0f, 0.0f,  1.0f, 0.0f,
	};

	// COPYRIGHTED IMAGES, DO NOT USE
	// (funny how they're mixing so smoothly)
	texture triTexture("assets/small_img.jpg");
	texture bigTexture("assets/img.jpg");
	if (triTexture.fail() || bigTexture.fail())
		return -1;

	glActiveTexture(GL_TEXTURE0);
	triTexture.bind();

	glActiveTexture(GL_TEXTURE1);
	bigTexture.bind();

	GLsizei stride = 5 * sizeof(float);

	mesh triangle(triangleData, sizeof(triangleData));
	triangle.vertexAttrib(0, 3, stride);
	triangle.vertexAttrib(1, 2, stride, (const void*)(3 * sizeof(float)));

	mesh triangle2(triangleData2, sizeof(triangleData2));
	triangle2.vertexAttrib(0, 3, stride);
	triangle.vertexAttrib(1, 2, stride, (const void*)(3 * sizeof(float)));

	shader baseShader("shaders/vertex.glsl", "shaders/fragment.glsl");
	if (baseShader.fail())
		return -1;

	baseShader.use();

	baseShader.setUniform(baseShader.getUniformLocation("tex"), 0);
	baseShader.setUniform(baseShader.getUniformLocation("tex2"), 1);

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
	* Initialise GLFW, GL3W and OpenGL
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

	glViewport(0, 0, 500, 500);

	// Texture settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return window;
}
