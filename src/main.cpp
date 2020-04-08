#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "bullet.hpp"
#include "camera.hpp"
#include "error.hpp"
#include "map.hpp"
#include "model.hpp"
#include "player.hpp"
#include "shader.hpp"

// @TODO: Make everything compliant with C++ Core Guidelines
// @TODO: Document everything
// @TODO: Make a fucking Game class, this is a real mess >:(

GLFWwindow* init_GL();
float calculate_cursor_angle(glm::mat4 VP, glm::vec3 player_pos);
// THIS IS A PLACEHOLDER. DON'T JUDGE ME, NOR DO THAT.
Map* map_ptr;

int main() {
	GLFWwindow* window = init_GL();
	if (window == nullptr)
		return -1;

	Shader base_shader("shaders/vertex.glsl", "shaders/fragment.glsl");
	if (base_shader.fail())
		return -1;

	base_shader.use();

	// Set up the camera to be aligned with the map
	Camera cam(glm::vec3(3.0f, 30.0f, 10.5f), glm::vec2(0.0f, -1.4f)); // 1.4 rad ~= 80 deg

	// This is BAD. Use a class.
	Map map = Map("assets/map_0.txt");
	map_ptr = &map;

	if (map.has_failed())
		return -1;

	Player player(map.get_player_starting_pos());

	GLint uniform_time = base_shader.get_uniform_location("time");
	GLint uniform_MVP = base_shader.get_uniform_location("MVP");

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float time = glfwGetTime();
		base_shader.use();
		base_shader.set_uniform(uniform_time, time);

		// WASD / ZQSD controls
		glm::vec3 player_pos = player.get_position();

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.1f);
			if (!map.collision_check(player_pos + offset)) {
				player.move(offset);
			}
		} else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			glm::vec3 offset = glm::vec3(0.0f, 0.0f, -0.1f);
			if (!map.collision_check(player_pos + offset)) {
				player.move(offset);
			}
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			glm::vec3 offset = glm::vec3(-0.1f, 0.0f, 0.0f);
			if (!map.collision_check(player_pos + offset)) {
				player.move(offset);
			}
		} else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			glm::vec3 offset = glm::vec3(0.1f, 0.0f, 0.0f);
			if (!map.collision_check(player_pos + offset)) {
				player.move(offset);
			}
		}

		BulletManager::tick(map);
		BulletManager::draw(base_shader, uniform_MVP, cam.get_VP());

		player.set_angle(calculate_cursor_angle(cam.get_VP(), player.get_position()));
		player.draw(base_shader, uniform_MVP, cam.get_VP());
		map.draw(base_shader, uniform_MVP, cam.get_VP());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

glm::vec2 cursor_pos = glm::vec2(0.0f);
float calculate_cursor_angle(glm::mat4 VP, glm::vec3 player_pos) {
	// World coordinates to screen coordinates
	glm::vec4 world_pos = VP * glm::vec4(player_pos, 1.0f);
	world_pos /= world_pos.w;
	float screen_x = (world_pos.x + 1) / 2.0f * 500.0f + 0.5f;
	float screen_y = (1 - world_pos.y) / 2.0f * 500.0f + 0.5f;

	// Set origin to player, and change to cartesian coordinate system
	float x = cursor_pos.x - screen_x;
	float y = screen_y - cursor_pos.y;

	return glm::atan(y, x);
}

void cursor_pos_callback(GLFWwindow*, double x, double y) {
	cursor_pos = glm::vec2(x, y);
}

float angle_test = 0.0f;
void mouse_btn_callback(GLFWwindow*, int btn, int action, int) {
	if (btn == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		angle_test += 0.1f;
		BulletManager::create(glm::vec2(5.0f, 3.0f), angle_test, *map_ptr);
	}
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
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_btn_callback);
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
