#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "bullet.hpp"
#include "game.hpp"
#include "log.hpp"
#include "map.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "tank.hpp"

namespace {
	Model bullet_mdl;
	std::vector<Bullet> bullets;
}

bool BulletManager::create(const glm::vec2& pos, float angle, const Map& map) {
	glm::vec3 position = glm::vec3(pos.x, 0.0f, pos.y);

	if (bullet_mdl.is_empty()) {
		bullet_mdl.load("models/bullet.obj");
	}

	if (map.collision_check(position) != Map_collision::none) {
		Log::error("Invalid bullet position");
		return false;
	}

	const float speed = 0.2f;
	Bullet bullet = {
		.velocity = glm::vec3(glm::sin(angle) * speed, 0.0f, glm::cos(angle) * speed),
		.position = position,
		.angle = angle,
		.remaining_hit = 3,
	};

	bullets.push_back(bullet);

	return true;
}

// Return collision angle with the wall at its origin
static float get_collision_angle(
	const Map& map,
	const glm::vec3& new_pos,
	const glm::vec3& old_pos
	) {

	Map_collision collision = map.collision_check(new_pos);

	if (collision == Map_collision::none) {
		return -100.0f;
	}

	if (collision == Map_collision::right_or_left) {
		return 0.0f;
	} else if (collision == Map_collision::up_or_down) {
		return glm::pi<float>();
	}

	float x, y;

	if (collision == Map_collision::upper_left || collision == Map_collision::bottom_left) {
		x = round(new_pos.z - 0.35f);
	} else {
		x = round(new_pos.z + 0.35f);
	}

	if (collision == Map_collision::upper_left || collision == Map_collision::upper_right) {
		y = round(new_pos.x - 0.35f);
	} else {
		y = round(new_pos.x + 0.35f);
	}

	x = old_pos.z - x;
	y = old_pos.x - y;

	return glm::atan(y, x);
}

static void draw(
	const Bullet& bullet,
	const Shader& shader,
	GLint uniform_MVP,
	const glm::mat4& VP
	) {

	bullet_mdl.set_angle(bullet.angle);
	bullet_mdl.set_pos(bullet.position);
	bullet_mdl.draw(shader, uniform_MVP, VP);
}

void BulletManager::frame(
	const Game& game,
	const Shader& shader,
	GLint uniform_MVP,
	const glm::mat4& VP
	) {

	for (auto bullet = bullets.begin(); bullet != bullets.end(); ++bullet) {
		glm::vec3 new_pos = bullet->position + bullet->velocity;

		if (TankManager::bullet_collision(new_pos)) {
			bullet = bullets.erase(bullet) - 1;
			continue;
		}

		float angle = get_collision_angle(game.get_map(), new_pos, bullet->position);
		if (angle == -100.0f) {
			bullet->position = new_pos;
			draw(*bullet, shader, uniform_MVP, VP);
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
		draw(*bullet, shader, uniform_MVP, VP);
	}
}
