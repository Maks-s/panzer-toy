#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "bullet.hpp"
#include "log.hpp"
#include "map.hpp"
#include "model.hpp"
#include "shader.hpp"

namespace BulletManager {
	namespace {
		const float speed = 0.2f;
		std::unique_ptr<Model> bullet_mdl;
		std::vector<Bullet> bullets;
	}
}

bool BulletManager::create(glm::vec2 pos, float angle, Map map) {
	glm::vec3 position = glm::vec3(pos.x, 1.0f, pos.y);

	if (!bullet_mdl) {
		Model mdl("models/bullet.obj");
		bullet_mdl = std::make_unique<Model>(mdl);
	}

	if (map.collision_check(position)) {
		Log::error("Invalid bullet position");
		return false;
	}

	Bullet bullet = {
		.velocity = glm::vec3(glm::sin(angle) * speed, 0.0f, glm::cos(angle) * speed),
		.position = position,
		.angle = angle,
		.remaining_hit = 10,
	};

	bullets.push_back(bullet);

	return true;
}

// Return collision angle with the wall at its origin
static float get_collision_angle(Map& map, glm::vec3 new_pos, glm::vec3 old_pos) {
	int collision = map.collision_check(new_pos);

	if (!collision) {
		return -100.0f;
	}

	float x, y;

	if (collision == 1 || collision == 2) {
		x = round(new_pos.z - 0.35f);
	} else {
		x = round(new_pos.z + 0.35f);
	}

	if (collision == 1 || collision == 4) {
		y = round(new_pos.x - 0.35f);
	} else {
		y = round(new_pos.x + 0.35f);
	}

	x = old_pos.z - x;
	y = old_pos.x - y;

	return glm::atan(y, x);
}

void BulletManager::tick(Map map) {
	for (int i=bullets.size() - 1; i >= 0; --i) {
		Bullet& bullet = bullets[i];

		glm::vec3 new_pos = bullet.position + bullet.velocity;
		float angle = get_collision_angle(map, new_pos, bullet.position);

		if (angle == -100.0f) {
			bullet.position = new_pos;
			continue;
		}

		if (!bullet.remaining_hit--) {
			bullets.erase(bullets.begin() + i);
		}

		// Check if the bullet hit the wall from right or left
		if (
			(angle > -M_PI_4 && angle < M_PI_4)
			|| (angle > -M_PI && angle < M_PI_4-M_PI)
			|| (angle > M_PI-M_PI_4 && angle < M_PI)
			) {

			bullet.velocity.z *= -1.0f;
		} else {
			bullet.velocity.x *= -1.0f;
		}

		bullet.angle = glm::atan(bullet.velocity.x, bullet.velocity.z);
	}
}

void BulletManager::draw(Shader shader, GLint uniform_MVP, glm::mat4 VP) {
	Model* mdl;
	mdl = bullet_mdl.get();

	for (auto& bullet : bullets) {
		mdl->set_angle(bullet.angle);
		mdl->set_position(bullet.position);
		mdl->draw(shader, uniform_MVP, VP);
	}
}
