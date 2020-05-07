#include "game.hpp" // Need to be included first to load gl3w
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>

#include "log.hpp"
#include "map.hpp"
#include "player.hpp"

// @TODO: Support controllers
void Player::handle_movement(const Game& game, GLFWwindow* window) {
	bool w_pressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
	bool a_pressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
	bool s_pressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	bool d_pressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

	const float pi = glm::pi<float>();
	const float half_pi = glm::half_pi<float>();
	const float quarter_pi = glm::quarter_pi<float>();

	if (
		(w_pressed && s_pressed)
		|| (d_pressed && a_pressed)
		|| !(w_pressed || a_pressed || s_pressed || d_pressed)
		) {

		tick();
		return;
	}

	if (w_pressed) {
		if (d_pressed) {
			set_direction(pi + quarter_pi);
		} else if (a_pressed) {
			set_direction(pi + half_pi + quarter_pi);
		} else {
			set_direction(pi + half_pi);
		}
	} else if (s_pressed) {
		if (d_pressed) {
			set_direction(half_pi + quarter_pi);
		} else if (a_pressed) {
			set_direction(quarter_pi);
		} else {
			set_direction(half_pi);
		}
	} else if (a_pressed) {
		set_direction(0.0f);
	} else if (d_pressed) {
		set_direction(pi);
	}

	if (get_remaining_steps() <= 0) {
		const float angle = get_base_angle();
		const float speed = 0.03f;

		glm::vec3 offset = glm::vec3(glm::sin(angle) * speed, 0.0f, glm::cos(angle) * speed);
		if (game.collision_check(get_pos() + offset) == MapCollision::none) {
			move(offset);
		}
	}

	tick();
}

bool Player::bullet_collision(Game& game, const glm::vec3& bullet_pos) {
	if (glm::distance2(bullet_pos, get_pos()) < 0.16f) {
		game.reset();
		return true;
	}

	return false;
}
