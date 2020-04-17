#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/glm.hpp>

#include "tank.hpp"

class Game;
class GLFWwindow;

class Player : public Tank {
public:
	Player() : Tank() {};
	Player(const glm::vec3& pos) : Tank(pos) {};

	void handle_movement(const Game& game, GLFWwindow* window);
};

#endif // PLAYER_HPP
