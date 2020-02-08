#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/glm.hpp>

#include "model.hpp"

class Player : public Model {
public:
	Player() : Model("models/nanosuit.obj") {};
	Player(glm::vec3 pos) : Player() { set_position(pos); };
};

#endif // PLAYER_HPP
