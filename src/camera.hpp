#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera {
public:
	Camera() {};

	Camera(
		const glm::vec3& _pos,
		const glm::vec2& _angle
	) : pos(_pos), angle(_angle) { calculate_VP(); };

	glm::mat4 get_VP() const { return VP; };

	glm::vec2 get_angle() const { return angle; };
	void set_angle(const glm::vec2& angle);
	void rotate(const glm::vec2& angle);

	glm::vec3 get_pos() const { return pos; };
	void set_pos(const glm::vec3& pos);
	void move(const glm::vec3& offset);

private:
	glm::vec3 pos = glm::vec3(0.0f);
	glm::vec2 angle = glm::vec2(0.0f); // yaw, pitch
	glm::mat4 VP = glm::mat4(0.0f); // projection * view, view isn't saved. See calculate_VP

	void calculate_VP();
};

#endif // CAMERA_HPP
