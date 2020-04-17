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
	std::vector<Tank> enemy_list;
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
	if (--steps >= 0) {
		if (clockwise) {
			rotate(speed * -1.0f);
		} else {
			rotate(speed);
		}
	}
}

void TankManager::create(const glm::vec3& pos) {
	enemy_list.push_back(Tank(pos));
}

void TankManager::frame(const Game& game, const Shader& shader, const glm::mat4& VP) {
	glm::vec3 tank_pos = game.get_player_pos();

	for (auto& enemy : enemy_list) {
		glm::vec3 foe_pos = enemy.get_pos();

		float x = tank_pos.z - foe_pos.z;
		float y = tank_pos.x - foe_pos.x;

		enemy.set_angle(glm::atan(y, x));
		enemy.shoot(game);
		enemy.tick();
		enemy.draw(shader, VP);
	}
}

bool TankManager::bullet_collision(const glm::vec3& bullet_pos) {
	for (auto enemy = enemy_list.begin(); enemy != enemy_list.end(); ++enemy) {
		glm::vec3 foe_pos = enemy->get_pos();

		if (glm::distance(bullet_pos, foe_pos) < 0.4f) {
			enemy = enemy_list.erase(enemy) - 1;
			return true;
		}
	}

	return false;
}
