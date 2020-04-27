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

// @TODO: Implement good AI

class BasicEnemy : public Enemy {
public:
	using Enemy::Enemy;

	void behavior(const Game& game, const glm::vec3& ply_pos) {
		glm::vec3 foe_pos = get_pos();

		float x = ply_pos.z - foe_pos.z;
		float y = ply_pos.x - foe_pos.x;

		set_base_angle(glm::atan(y, x));
		shoot(game);
	}
};

class CrazyEnemy : public Enemy {
public:
	using Enemy::Enemy;

	void behavior(const Game& game, const glm::vec3&) {
		if (--steps <= 0) {
			steps = smooth_turn_angle(get_top_angle(), rdm() % 6, speed, clockwise);
		} else {
			float new_angle;
			if (clockwise) {
				new_angle = get_top_angle() + speed;
			} else {
				new_angle = get_top_angle() - speed;
			}

			set_top_angle(new_angle);
		}

		shoot(game);
		tick();
	}

private:
	int steps = 0;
	bool clockwise = false;
	const float speed = 0.1f;
};

void EnemyManager::init() {
	rdm.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

void EnemyManager::create(const glm::vec3& pos, EnemyType type) {
	switch (type) {
	case EnemyType::crazy:
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
