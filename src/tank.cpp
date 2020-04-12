#include <memory>
#include <forward_list>
#include <glm/glm.hpp>

#include "bullet.hpp"
#include "game.hpp"
#include "map.hpp"
#include "shader.hpp"
#include "tank.hpp"

namespace TankManager {
	namespace {
		std::forward_list<std::unique_ptr<Tank>> enemy_list;
	}
}

void Tank::shoot(Game* game) {
	const float time = game->get_current_time();
	if (time - last_shoot_time < 2.0f)
		return;

	const float offset = 0.8f;
	const float angle = get_angle();
	glm::vec3 ply_pos = get_position();
	glm::vec2 pos(glm::sin(angle) * offset + ply_pos.x, glm::cos(angle) * offset + ply_pos.z);

	BulletManager::create(pos, angle, game->get_map());
	last_shoot_time = time;
}

void TankManager::create(glm::vec3 pos) {
	enemy_list.push_front(std::make_unique<Tank>(pos));
}

void TankManager::frame(Game* game, Shader shader, GLint uniform_MVP, glm::mat4 VP) {
	glm::vec3 ply_pos = game->get_player_pos();

	for (auto& enemy : enemy_list) {
		glm::vec3 foe_pos = enemy->get_position();

		float x = ply_pos.z - foe_pos.z;
		float y = ply_pos.x - foe_pos.x;

		enemy->set_angle(glm::atan(y, x));
		enemy->shoot(game);
		enemy->draw(shader, uniform_MVP, VP);
	}
}

bool TankManager::bullet_collision(glm::vec3 bullet_pos) {
	for (auto& enemy : enemy_list) {
		glm::vec3 foe_pos = enemy->get_position();

		if (glm::distance(bullet_pos, foe_pos) < 0.4f) {
			enemy_list.remove(enemy);
			return true;
		}
	}

	return false;
}
