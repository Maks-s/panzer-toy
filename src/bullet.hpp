#ifndef BULLET_HPP
#define BULLET_HPP

#include <GL/gl3w.h>
#include <glm/glm.hpp>

class Game;
class Map;
class Shader;

struct Bullet {
	glm::vec3 velocity; // offset by which the bullet moves
	glm::vec3 position;
	float angle;
	int remaining_hit;
};

namespace BulletManager {
	bool create(const glm::vec2& pos, float angle, const Map& map);
	void frame(const Game& game, const Shader& shader, GLint uniform_MVP, const glm::mat4& VP);
}

#endif // BULLET_HPP
