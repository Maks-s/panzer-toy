#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"

void Camera::set_ratio(float ratio) {
	// Ratio of the arena is 14:11
	if (ratio < 1.2727f) {
		const float height = 14.0f / ratio;
		projection = glm::ortho(-14.0f, 14.0f, -height, height, 0.1f, 100.0f);

		calculate_VP();
		return;
	}

	const float width = ratio * 11.0f;
	projection = glm::ortho(-width, width, -11.0f, 11.0f, 0.1f, 100.0f);

	calculate_VP();
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
