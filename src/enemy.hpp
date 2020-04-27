#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <glm/glm.hpp>
#include "tank.hpp"

class Game;
class Shader;

enum class EnemyType : int {
	basic = 5,
	crazy = 6
};

class Enemy : public Tank {
public:
	using Tank::Tank;
	virtual ~Enemy() = default;

	virtual void behavior(const Game&, const glm::vec3&) = 0;
};

namespace EnemyManager {
	void init();
	void create(const glm::vec3& pos, EnemyType type);
	void frame(const Game& game, const Shader& shader, const glm::mat4& VP);
	bool bullet_collision(const glm::vec3& bullet_pos);
}

#endif // ENEMY_HPP
