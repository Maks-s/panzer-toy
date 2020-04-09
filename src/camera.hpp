#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera {
public:
	Camera() { calculate_VP(); };

	Camera(
		glm::vec3 _pos,
		glm::vec2 _angle
	) : pos(_pos), angle(_angle) { calculate_VP(); };

	glm::mat4 get_VP() const { return VP; };

	void set_angle(glm::vec2 angle);
	void rotate(glm::vec2 angle);

	void set_position(glm::vec3 pos);
	void move(glm::vec3 offset);

private:
	glm::vec3 pos = glm::vec3(0.0f);
	glm::vec2 angle = glm::vec2(0.0f); // yaw, pitch
	glm::mat4 VP; // projection * view, view isn't saved. See calculate_VP

	void calculate_VP();
};

#endif // CAMERA_HPP
