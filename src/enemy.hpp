#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <glm/glm.hpp>
#include "tank.hpp"

class Game;
class Shader;

class Enemy : public Tank {

};

namespace EnemyManager {
	void create(const glm::vec3& pos);
	void frame(const Game& game, const Shader& shader, const glm::mat4& VP);
	bool bullet_collision(const glm::vec3& bullet_pos);
}

#endif // ENEMY_HPP
