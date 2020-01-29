#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "error.hpp"
#include "model.hpp"
#include "shader.hpp"

// @TODO: Make everything compliant with C++ Core Guidelines

GLFWwindow* init_GL();

int main() {
	GLFWwindow* window = init_GL();
	if (window == nullptr)
		return -1;

	Shader base_shader("shaders/vertex.glsl", "shaders/fragment.glsl");
	if (base_shader.fail())
		return -1;

	base_shader.use();

	Model mdl("models/nanosuit.obj");
	Camera cam(glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(0.0f), 45.0f);

	GLint uniform_time = base_shader.get_uniform_location("time");
	GLint uniform_MVP = base_shader.get_uniform_location("MVP");

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float time = glfwGetTime();

		cam.rotate(glm::vec3(0.01f, 0.0f, 0.01f));

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::rotate(model, time, glm::vec3(0.0f, 1.0f, 0.0f));

		base_shader.use();
		base_shader.set_uniform(uniform_time, time);
		base_shader.set_uniform(uniform_MVP, cam.get_VP() * model);

		mdl.draw(base_shader);

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
GLFWwindow* init_GL() {
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
	glEnable(GL_DEPTH_TEST);

	// Texture settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return window;
}
