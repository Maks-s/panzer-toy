#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"

namespace {
	const glm::mat4 projection = glm::ortho(-14.0f, 14.0f, -14.0f, 14.0f, 0.1f, 100.0f);
	// const glm::mat4 projection = glm::perspective(1.57f, 1.0f, 0.1f, 100.0f); // Used for debugging purpose
}

void Camera::calculate_VP() {
	glm::vec3 direction = glm::vec3(
		glm::cos(angle.x) * glm::cos(angle.y),
		glm::sin(angle.y),
		glm::sin(angle.x) * glm::cos(angle.y)
	);

	glm::mat4 view = glm::lookAt(pos, pos + direction, glm::vec3(0.0f, 1.0f, 0.0f));

	VP = projection * view;
}

void Camera::set_angle(const glm::vec2& angle) {
	this->angle = angle;
	calculate_VP();
}

void Camera::rotate(const glm::vec2& angle) {
	this->angle += angle;
	calculate_VP();
}

void Camera::set_pos(const glm::vec3& pos) {
	this->pos = pos;
	calculate_VP();
}

void Camera::move(const glm::vec3& offset) {
	pos += offset;
	calculate_VP();
}
