#ifndef PANZERTOY_PLAYER_HPP
#define PANZERTOY_PLAYER_HPP

#include <glm/glm.hpp>

#include "tank.hpp"

class Game;
class GLFWwindow;

class Player : public Tank {
public:
	using Tank::Tank;

	void handle_movement(const Game& game, GLFWwindow* window);
	bool bullet_collision(Game& game, const glm::vec3& bullet_pos);
};

#endif // PANZERTOY_PLAYER_HPP
