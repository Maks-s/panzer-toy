#include <glm/glm.hpp>

#include "enemy.hpp"
#include "game.hpp"
#include "shader.hpp"

namespace {
	std::vector<Tank> enemy_list;
}

void EnemyManager::create(const glm::vec3& pos) {
	enemy_list.push_back(Tank(pos));
}

void EnemyManager::frame(const Game& game, const Shader& shader, const glm::mat4& VP) {
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

bool EnemyManager::bullet_collision(const glm::vec3& bullet_pos) {
	for (auto enemy = enemy_list.begin(); enemy != enemy_list.end(); ++enemy) {
		glm::vec3 foe_pos = enemy->get_pos();

		if (glm::distance(bullet_pos, foe_pos) < 0.4f) {
			enemy = enemy_list.erase(enemy) - 1;
			return true;
		}
	}

	return false;
}
