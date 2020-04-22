#include "game.hpp"
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "bullet.hpp"
#include "map.hpp"
#include "shader.hpp"
#include "tank.hpp"

namespace {
	const float pi = glm::pi<float>();
}

void Tank::shoot(const Game& game) {
	const float time = game.get_current_time();
	if (time - last_shoot_time < 2.0f)
		return;

	const float offset = 0.8f;
	const float angle = get_bone_angle() + get_angle();
	glm::vec3 tank_pos = get_pos();
	glm::vec2 pos(glm::sin(angle) * offset + tank_pos.x, glm::cos(angle) * offset + tank_pos.z);

	if (BulletManager::create(pos, angle, game)) {
		last_shoot_time = time;
	}
}

int Tank::smooth_turn_angle(float from, float to, float speed, bool& clockwise) {
	from = glm::mod(from, 2*pi);
	float distance = glm::abs(from - to);

	if (to > from) {
		if (distance > pi) {
			clockwise = true;
			distance = from + 2 * pi - to;
		} else {
			clockwise = false;
		}
	} else {
		if (distance > pi) {
			clockwise = false;
			distance = to + 2 * pi - from;
		} else {
			clockwise = true;
		}
	}

	return glm::round(distance / speed);
}

void Tank::set_direction(float direction) {
	steps = smooth_turn_angle(get_angle(), direction, speed, clockwise);
}

void Tank::tick() {
	if (steps > 0) {
		steps--;

		if (clockwise) {
			rotate(-speed);
		} else {
			rotate(speed);
		}
	}
}
