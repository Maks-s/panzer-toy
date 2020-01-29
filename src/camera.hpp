#include <glm/glm.hpp>

class Camera {
public:
	Camera(
		glm::vec3 _pos = glm::vec3(0.0f),
		glm::vec3 _angle = glm::vec3(0.0f),
		float fov = 45.0f
	) : pos(_pos), angle(_angle) { set_fov(fov); };

	Camera(float fov) : Camera(glm::vec3(0.0f), glm::vec3(0.0f), fov) {};

	glm::mat4 get_VP() const { return VP; };

	void set_fov(float fov); // FOV in degrees

	// Angle is in radian !
	void set_angle(glm::vec3 angle);
	void rotate(glm::vec3 angle);

	void set_position(glm::vec3 pos);
	void move(glm::vec3 offset);

private:
	glm::vec3 pos;
	glm::vec3 angle; // yaw, pitch, roll in radians
	glm::mat4 projection;
	glm::mat4 VP; // projection * view, view isn't saved. See calculate_VP

	void calculate_VP();
};
