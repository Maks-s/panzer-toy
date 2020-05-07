#include "game.hpp"
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "bullet.hpp"
#include "log.hpp"
#include "map.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "enemy.hpp"

namespace {
	Model bullet_mdl;
	std::vector<Bullet> bullets;
}

bool BulletManager::create(const glm::vec3& pos, float angle, const Game& game) {
	if (bullet_mdl.is_empty()) {
		bullet_mdl.load("models/bullet.dae");
	}

	if (game.collision_check(pos) != MapCollision::none) {
		Log::error("Invalid bullet position");
		return false;
	}

	const float speed = 0.2f;
	Bullet bullet = {
		.velocity = glm::vec3(glm::sin(angle) * speed, 0.0f, glm::cos(angle) * speed),
		.position = pos,
		.angle = angle,
		.remaining_hit = 3,
	};

	bullets.push_back(bullet);

	return true;
}

// Return collision angle with the wall at its origin
static float get_collision_angle(
		const Game& game,
		const glm::vec3& new_pos,
		const glm::vec3& old_pos
	) {

	MapCollision collision = game.collision_check(new_pos);

	if (collision == MapCollision::none) {
		return -100.0f;
	}

	if (collision == MapCollision::right_or_left) {
		return 0.0f;
	} else if (collision == MapCollision::up_or_down) {
		return glm::pi<float>();
	}

	float x, y;

	if (collision == MapCollision::upper_left || collision == MapCollision::bottom_left) {
		x = round(new_pos.z - 0.35f);
	} else {
		x = round(new_pos.z + 0.35f);
	}

	if (collision == MapCollision::upper_left || collision == MapCollision::upper_right) {
		y = round(new_pos.x - 0.35f);
	} else {
		y = round(new_pos.x + 0.35f);
	}

	x = old_pos.z - x;
	y = old_pos.x - y;

	return glm::atan(y, x);
}

static void draw(const Bullet& bullet, const Shader& shader, const glm::mat4& VP) {
	bullet_mdl.set_angle(bullet.angle);
	bullet_mdl.set_pos(bullet.position);
	bullet_mdl.draw(shader, VP);
}

void BulletManager::frame(Game& game, const Shader& shader, const glm::mat4& VP) {
	for (auto bullet = bullets.begin(); bullet != bullets.end(); ++bullet) {
		glm::vec3 new_pos = bullet->position + bullet->velocity;

		if (game.player_bullet_collision(new_pos)) {
			return; // Game has been reset, return
		}

		if (EnemyManager::bullet_collision(new_pos)) {
			bullet = bullets.erase(bullet) - 1;
			continue;
		}

		float angle = get_collision_angle(game, new_pos, bullet->position);
		if (angle == -100.0f) {
			bullet->position = new_pos;
			draw(*bullet, shader, VP);
			continue;
		}

		if (!bullet->remaining_hit--) {
			bullet = bullets.erase(bullet) - 1;
			continue;
		}

		// Check if the bullet hit the wall from right or left
		const float pi = glm::pi<float>();
		const float pi_4 = glm::quarter_pi<float>();
		if (
			(angle > -pi_4 && angle < pi_4)
			|| (angle > -pi && angle < pi_4-pi)
			|| (angle > pi-pi_4 && angle < pi)
			) {

			bullet->velocity.z *= -1.0f;
		} else {
			bullet->velocity.x *= -1.0f;
		}

		bullet->angle = glm::atan(bullet->velocity.x, bullet->velocity.z);
		draw(*bullet, shader, VP);
	}
}

void BulletManager::clear() {
	bullets.clear();
}
