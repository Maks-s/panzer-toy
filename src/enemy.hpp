#ifndef PANZERTOY_ENEMY_HPP
#define PANZERTOY_ENEMY_HPP

#include <glm/glm.hpp>
#include "tank.hpp"

class Game;
class Shader;

/**
 * @enum EnemyType
 *
 * @brief Define identifiers for enemy types
 *
 * Identifiers are used in maps to spawn enemies.
 * It must start at 5, because 0 to 4 are reserved by map objects
 */
enum class EnemyType : int {
	basic = 5,
	crazy = 6
};

/**
 * @class Enemy
 *
 * @brief Abstract class for enemies
 */
class Enemy : public Tank {
public:
	using Tank::Tank;
	virtual ~Enemy() = default;

	/**
	 * @brief AI Behavior
	 *
	 * This function is ran each frame to calculate what the enemy will do:
	 * Turn, shoot, move...
	 */
	virtual void behavior(const Game&, const glm::vec3&) = 0;
};

/**
 * @namespace EnemyManager
 *
 * @brief Manage enemies and enemies' AI
 *
 * Also manage collisions between enemies and bullets
 */
namespace EnemyManager {
	void init();
	void create(const glm::vec3& pos, EnemyType type);
	void frame(const Game& game, const Shader& shader, const glm::mat4& VP);
	bool bullet_collision(Game& game, const glm::vec3& bullet_pos);
	void set_global_last_shoot_time(float time);
	bool empty();
	void clear();
}

#endif // PANZERTOY_ENEMY_HPP
