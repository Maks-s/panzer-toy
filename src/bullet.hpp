#ifndef BULLET_HPP
#define BULLET_HPP

#include <gl/gl3w.h>
#include <glm/glm.hpp>

#include "map.hpp"
#include "model.hpp"
#include "shader.hpp"

struct Bullet {
	glm::vec3 velocity; // offset by which the bullet moves
	glm::vec3 position;
	float angle;
	int remaining_hit;
};

namespace BulletManager {
	bool create(glm::vec2 pos, float angle, Map map);
	void tick(Map map);
	void draw(Shader shader, GLint uniform_MVP, glm::mat4 VP);
}

#endif // BULLET_HPP