#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "mesh.hpp"
#include "shader.hpp"

class Model {
public:
	Model(std::string path);
	void draw(Shader shader, GLint uniform_MVP, glm::mat4 VP);

	void set_position(glm::vec3 pos);
	void move(glm::vec3 pos);

	void set_angle(float angle);
	void rotate(float angle);

	glm::vec3 get_position() { return position; };
	float get_angle() { return angle; };

private:
	std::vector<Mesh> meshes;
	glm::mat4 model_mat;
	glm::vec3 position = glm::vec3(0.0f);
	float angle = 0.0f; // angle is in radians
	bool dirty;
};

#endif // MODEL_HPP
