#include <chrono>
#include <memory>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "enemy.hpp"
#include "game.hpp"
#include "shader.hpp"

namespace {
	std::vector<std::unique_ptr<Enemy>> enemy_list;
	std::minstd_rand rdm;
}

// @TODO: Implement good AI
// @TODO: Make color-coded enemies

/**
 * @class BasicEnemy
 *
 * @brief Its cannon is locked onto your position
 */
class BasicEnemy : public Enemy {
public:
	using Enemy::Enemy;

	void behavior(const Game& game, const glm::vec3& ply_pos) {
		glm::vec3 foe_pos = get_pos();

		float x = ply_pos.z - foe_pos.z;
		float y = ply_pos.x - foe_pos.x;

		set_top_angle(glm::atan(y, x));
		shoot(game);
	}
};

/**
 * @class CrazyEnemy
 *
 * @brief Shoot randomly in random directions
 *
 * @note I wanted to name it "Terrorist", but crazy is more kiddy-friendly
 */
class CrazyEnemy : public Enemy {
public:
	using Enemy::Enemy;

	void behavior(const Game& game, const glm::vec3&) {
		if (--steps <= 0) {
			steps = calculate_rotation_steps(get_top_angle(), rdm() % 6, speed, clockwise);
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
		tick_base_rotation();
	}

private:
	int steps = 0;
	bool clockwise = false; // Turn clockwise or counterclockwise
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

/** @brief Function called each frame */
void EnemyManager::frame(const Game& game, const Shader& shader, const glm::mat4& VP) {
	glm::vec3 ply_pos = game.get_player_pos();

	for (auto& enemy : enemy_list) {
		enemy->behavior(game, ply_pos);
		enemy->draw(shader, VP);
	}
}

/**
 * @brief Check if a bullet hit an enemy
 *
 * @return True if it hit, false otherwise
 */
bool EnemyManager::bullet_collision(Game& game, const glm::vec3& bullet_pos) {
	for (auto ptr = enemy_list.begin(); ptr != enemy_list.end(); ++ptr) {
		Enemy* enemy = ptr->get();

		glm::vec3 foe_pos = enemy->get_pos();

		// Distance without sqrt, so it's faster but squared
		if (glm::distance2(bullet_pos, foe_pos) < 0.16f) {
			ptr = enemy_list.erase(ptr) - 1;

			if (enemy_list.empty()) {
				game.finish_level();
			}

			return true;
		}
	}

	return false;
}

/** @brief Set the last shoot time for all spawned enemies */
void EnemyManager::set_global_last_shoot_time(float time) {
	for (auto& enemy : enemy_list) {
		enemy->set_last_shoot_time(time);
	}
}

bool EnemyManager::empty() {
	return enemy_list.empty();
}

void EnemyManager::clear() {
	enemy_list.clear();
}
