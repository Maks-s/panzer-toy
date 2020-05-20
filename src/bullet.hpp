#ifndef PANZERTOY_BULLET_HPP
#define PANZERTOY_BULLET_HPP

#include <GL/gl3w.h>
#include <glm/glm.hpp>

class Game;
class Shader;

struct Bullet {
	glm::vec3 velocity; // offset by which the bullet moves
	glm::vec3 position;
	float angle;
	int remaining_hit;
};

/**
 * @namespace BulletManager
 *
 * @brief Manage bullets and bullets collision
 */
namespace BulletManager {
	bool create(const glm::vec3& pos, float angle, const Game& game);
	void frame(Game& game, const Shader& shader, const glm::mat4& VP);
	void clear();
}

#endif // PANZERTOY_BULLET_HPP
