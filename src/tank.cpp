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
	const float angle = get_angle();
	glm::vec3 tank_pos = get_pos();
	glm::vec2 pos(glm::sin(angle) * offset + tank_pos.x, glm::cos(angle) * offset + tank_pos.z);

	if (BulletManager::create(pos, angle, game)) {
		last_shoot_time = time;
	}
}

void Tank::set_direction(float direction) {
	const float angle = glm::mod(get_angle(), 2*pi);
	float distance = glm::abs(angle - direction);

	if (direction > angle) {
		if (distance > pi) {
			clockwise = true;
			distance = angle + 2 * pi - direction;
		} else {
			clockwise = false;
		}
	} else {
		if (distance > pi) {
			clockwise = false;
			distance = direction + 2 * pi - angle;
		} else {
			clockwise = true;
		}
	}

	steps = distance / speed;
}

void Tank::tick() {
	if (steps > 0) {
		steps--;

		if (clockwise) {
			rotate(speed * -1.0f);
		} else {
			rotate(speed);
		}
	}
}
