#include "game.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "bullet.hpp"
#include "log.hpp"
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
 * @brief Calculate angle to turn to go from "from" to "to" (africa)
 *
 * @param from Starting angle
 * @param to Final angle
 *
 * @return Rotation angle
 */
float Tank::calculate_turn_angle(float from, float to) {
	from = glm::mod(from, 2*pi);
	float distance = glm::abs(from - to);

	if (to > from) {
		if (distance > pi) {
			distance = -(2 * pi + from - to);
		}
	} else {
		if (distance > pi) {
			distance = (2 * pi + to - from);
		} else {
			distance = -distance;
		}
	}

	return distance;
}

/** @brief Set wanted direction */
void Tank::set_direction(float direction) {
	remaining_angle = calculate_turn_angle(angle_base, direction);
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
void Tank::tick_base_rotation(float delta_time) {
	if (remaining_angle != 0.0f) {
		const float sign = glm::sign(remaining_angle);
		float offset = sign * speed * delta_time;

		remaining_angle -= offset;

		// Prevent exceeding target angle
		if ((sign == 1.0f && remaining_angle < 0.0f) || (sign == -1.0f && remaining_angle > 0.0f)) {
			offset += remaining_angle;
			remaining_angle = 0.0f;
		}

		angle_base += offset;
		angle_base = glm::mod(angle_base, 2*pi);
	}
}
