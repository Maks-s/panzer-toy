#include <chrono>
#include <memory>
#include <random>
#include <glm/glm.hpp>

#include "enemy.hpp"
#include "game.hpp"
#include "shader.hpp"

namespace {
	std::vector<std::unique_ptr<Enemy>> enemy_list;
	std::minstd_rand rdm;
}

// @TODO: Implement AI

class BasicEnemy : public Enemy {
public:
	using Enemy::Enemy;

	void behavior(const Game& game, const glm::vec3& ply_pos) {
		glm::vec3 foe_pos = get_pos();

		float x = ply_pos.z - foe_pos.z;
		float y = ply_pos.x - foe_pos.x;

		set_angle(glm::atan(y, x));
		shoot(game);
	}
};

class CrazyEnemy : public Enemy {
public:
	using Enemy::Enemy;

	void behavior(const Game& game, const glm::vec3& ply_pos) {
		if (get_remaining_steps() <= 0) {
			set_direction(rdm() % 6);
		}

		shoot(game);
		tick();
	}
};

void EnemyManager::init() {
	rdm.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

void EnemyManager::create(const glm::vec3& pos, Enemy_type type) {
	switch (type) {
	case Enemy_type::crazy:
		enemy_list.push_back(std::make_unique<CrazyEnemy>(pos));
		break;
	default:
		enemy_list.push_back(std::make_unique<BasicEnemy>(pos));
		break;
	}
}

void EnemyManager::frame(const Game& game, const Shader& shader, const glm::mat4& VP) {
	glm::vec3 ply_pos = game.get_player_pos();

	for (auto& enemy : enemy_list) {
		enemy->behavior(game, ply_pos);
		enemy->draw(shader, VP);
	}
}

bool EnemyManager::bullet_collision(const glm::vec3& bullet_pos) {
	for (auto ptr = enemy_list.begin(); ptr != enemy_list.end(); ++ptr) {
		Enemy* enemy = ptr->get();

		glm::vec3 foe_pos = enemy->get_pos();

		if (glm::distance(bullet_pos, foe_pos) < 0.4f) {
			ptr = enemy_list.erase(ptr) - 1;
			return true;
		}
	}

	return false;
}
