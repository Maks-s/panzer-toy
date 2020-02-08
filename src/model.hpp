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

private:
	std::vector<Mesh> meshes;
	glm::mat4 model_mat;
	glm::vec3 position = glm::vec3(0.0f);
};

#endif // MODEL_HPP
