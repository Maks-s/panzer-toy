#include "game.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "bullet.hpp"
#include "map.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "tank.hpp"

namespace {
	const float pi = glm::pi<float>();
	Model tank_top_mdl;
	Model tank_base_mdl;
}

void Tank::init() {
	tank_top_mdl.load("models/tank_top.dae");
	tank_base_mdl.load("models/tank_base.dae");
}

/** @brief Make the tank fire a bullet */
void Tank::shoot(const Game& game) {
	const float time = game.get_current_time();
	if (time - last_shoot_time < 2.0f) {
		return;
	}

	const float offset = 0.8f;
	glm::vec3 position(
		glm::sin(angle_top) * offset + pos.x,
		0.0f,
		glm::cos(angle_top) * offset + pos.z
	);

	if (BulletManager::create(position, angle_top, game)) {
		last_shoot_time = time;
	}
}

/**
 * @brief Calculate number of steps needed to rotate with the given speed
 *
 * @param[in] from Starting angle
 * @param[in] to Final angle
 * @param[in] speed Speed of the rotation, in rad/s
 * @param[out] clockwise_out True if the rotation is clockwise, false otherwise
 *
 * @return Number of steps, with 1 step = 1/60 of a second
 */
int Tank::calculate_rotation_steps(float from, float to, float speed, bool& clockwise_out) {
	from = glm::mod(from, 2*pi);
	float distance = glm::abs(from - to);

	if (to > from) {
		if (distance > pi) {
			clockwise_out = true;
			distance = from + 2 * pi - to;
		} else {
			clockwise_out = false;
		}
	} else {
		if (distance > pi) {
			clockwise_out = false;
			distance = to + 2 * pi - from;
		} else {
			clockwise_out = true;
		}
	}

	return glm::round(distance / speed);
}

/** @brief Set wanted direction */
void Tank::set_direction(float direction) {
	steps = calculate_rotation_steps(angle_base, direction, speed, clockwise);
}

void Tank::draw(const Shader& shader, const glm::mat4& VP) const {
	// We could use bones, but it's faster and cheaper to use 2 models
	tank_top_mdl.set_pos(glm::vec3(pos.x, pos.y + 0.2f, pos.z));
	tank_base_mdl.set_pos(pos);

	tank_base_mdl.set_angle(angle_base);
	tank_top_mdl.set_angle(angle_top);

	tank_top_mdl.draw(shader, VP);
	tank_base_mdl.draw(shader, VP);
}

/** @brief Make progress toward the wanted direction each tick */
void Tank::tick_base_rotation() {
	if (steps > 0) {
		steps--;

		if (clockwise) {
			angle_base -= speed;
		} else {
			angle_base += speed;
		}
	}
}
