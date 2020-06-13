#ifndef PANZERTOY_TANK_HPP
#define PANZERTOY_TANK_HPP

#include <glm/glm.hpp>

class Game;
class Shader;

/**
 * @class Tank
 *
 * @brief Represent a tank on the map, be it the player's or an enemy
 */
class Tank {
public:
	Tank() = default;
	Tank(const glm::vec3& _pos) : pos(_pos) {};
	virtual ~Tank() = default;

	static void init();

	void shoot(const Game& game);
	void tick_base_rotation(float delta_time);
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
	float get_remaining_angle() const { return remaining_angle; };

	static float calculate_turn_angle(float from, float to);

private:
	glm::vec3 pos = glm::vec3(0.0f);
	float angle_base = 0.0f;
	float angle_top = 0.0f;
	float last_shoot_time = 0.0f;
	const float speed = 3.6f; // How much radians to turn in 1s
	float remaining_angle = 0.0f; // Remaining angle to turn
};

#endif // PANZERTOY_TANK_HPP
