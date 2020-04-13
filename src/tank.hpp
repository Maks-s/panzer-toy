#ifndef TANK_HPP
#define TANK_HPP

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "model.hpp"

class Game;
class Shader;

// @TODO: Separate Player and Enemy

class Tank : public Model {
public:
	Tank() : Model("models/tank.obj") {};
	Tank(const glm::vec3& pos) : Tank() { set_pos(pos); };

	void shoot(const Game& game);
	float get_last_shoot_time() const { return last_shoot_time; };
	void set_last_shoot_time(float time) { last_shoot_time = time; };

private:
	float last_shoot_time = 2.0f;
};

namespace TankManager {
	void create(const glm::vec3& pos);
	void frame(const Game& game, const Shader& shader, GLint uniform_MVP, const glm::mat4& VP);
	bool bullet_collision(const glm::vec3& bullet_pos);
}

#endif // TANK_HPP
