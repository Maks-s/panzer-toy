#ifndef PANZERTOY_TANK_HPP
#define PANZERTOY_TANK_HPP

#include <glm/glm.hpp>

#include "model.hpp"

class Game;
class Shader;

class Tank {
public:
	Tank();
	Tank(const glm::vec3& pos) : Tank() { this->pos = pos; };
	virtual ~Tank() = default;

	void shoot(const Game& game);
	void tick();
	void draw(const Shader& shader, const glm::mat4& VP) const;

	float get_last_shoot_time() const { return last_shoot_time; };
	void set_last_shoot_time(float time) { last_shoot_time = time; };

	float get_top_angle() const { return angle_top; };
	void set_top_angle(float angle) { angle_top = angle; };

	float get_base_angle() const { return angle_base; };
	void set_base_angle(float angle) { angle_base = angle; };

	glm::vec3 get_pos() const { return pos; };
	void move(const glm::vec3& offset) { pos += offset; };
	void set_pos(const glm::vec3& pos) { this->pos = pos; };

	// Indicate where the tank wants to turn
	void set_direction(float new_direction);
	int get_remaining_steps() const { return steps; };

	static int smooth_turn_angle(float from, float to, float speed, bool& clockwise_out);

private:
	glm::vec3 pos = glm::vec3(0.0f);
	float angle_base = 0.0f;
	float angle_top = 0.0f;
	float last_shoot_time = 0.0f;
	float speed = 0.06f; // How much radians to turn in 1/60s
	int steps = 0; // Steps before finishing turning
	bool clockwise = false;
};

#endif // PANZERTOY_TANK_HPP
