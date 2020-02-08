#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"

void Camera::calculate_VP() {
	glm::vec3 direction = glm::vec3(
		std::cos(angle.x) * std::cos(angle.y),
		std::sin(angle.y),
		std::sin(angle.x) * std::cos(angle.y)
	);

	glm::mat4 view = glm::lookAt(pos, pos + direction, glm::vec3(0.0f, 1.0f, 0.0f));

	VP = projection * view;
}

void Camera::set_fov(float fov) {
	projection = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 100.0f);
	calculate_VP();
}

void Camera::set_angle(glm::vec3 angle) {
	this->angle = angle;
	calculate_VP();
}

void Camera::rotate(glm::vec3 angle) {
	// @TODO
	// I'm lazy
	this->angle += angle;
	calculate_VP();
}

void Camera::set_position(glm::vec3 pos) {
	this->pos = pos;
	calculate_VP();
}

void Camera::move(glm::vec3 offset) {
	pos += offset;
	calculate_VP();
}
