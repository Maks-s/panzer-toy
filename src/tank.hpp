#ifndef TANK_HPP
#define TANK_HPP

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "model.hpp"

class Game;
class Map;
class Shader;

// @TODO: Separate Player and Enemy

class Tank : public Model {
public:
	Tank() : Model("models/tank.obj") {};
	Tank(glm::vec3 pos) : Tank() { set_position(pos); };

	void shoot(Game* game);
	float get_last_shoot_time() { return last_shoot_time; };
	void set_last_shoot_time(float time) { last_shoot_time = time; };

private:
	float last_shoot_time = 2.0f;
};

namespace TankManager {
	void create(glm::vec3 pos);
	void frame(Game* game, Shader shader, GLint uniform_MVP, glm::mat4 VP);
	bool bullet_collision(glm::vec3 bullet_pos);
}

#endif // TANK_HPP
