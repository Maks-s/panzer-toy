#include <chrono>
#include <memory>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "enemy.hpp"
#include "game.hpp"
#include "log.hpp"
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
		if (remaining_angle == 0.0f) {
			remaining_angle = calculate_turn_angle(
				get_top_angle(),
				static_cast<int>(rdm()) % 6 - 3 // [-3; 3]
			);
		} else {
			const float sign = glm::sign(remaining_angle);
			float offset = sign * speed * game.get_delta_time();

			remaining_angle -= offset;

			// Prevent exceeding target angle
			if (
				(sign == 1.0f && remaining_angle < 0.0f)
				|| (sign == -1.0f && remaining_angle > 0.0f)
				) {
				offset += remaining_angle;
				remaining_angle = 0.0f;
			}

			const float new_angle = get_top_angle() + offset;

			set_top_angle(new_angle);
		}

		shoot(game);
	}

private:
	float remaining_angle = 0.0f; /**< @see Tank::remaining_angle */
	const float speed = 5.0f;
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
