#ifndef TANK_HPP
#define TANK_HPP

#include <glm/glm.hpp>

#include "animatedmodel.hpp"

class Game;
class Shader;

class Tank : public AnimatedModel {
public:
	Tank() : AnimatedModel("models/tank.dae") {};
	Tank(const glm::vec3& pos) : Tank() { set_pos(pos); };
	virtual ~Tank() = default;

	void shoot(const Game& game);
	void tick();

	float get_last_shoot_time() const { return last_shoot_time; };
	void set_last_shoot_time(float time) { last_shoot_time = time; };

	// Indicate where the tank wants to turn
	void set_direction(float new_direction);
	static int smooth_turn_angle(float from, float to, float speed, bool& clockwise);
	int get_remaining_steps() const { return steps; };

private:
	float last_shoot_time = 2.0f;
	int steps = 0; // Steps before finishing turning
	float speed = 0.06f; // How much radians to turn in 1/60s
	bool clockwise = false;
};

#endif // TANK_HPP
