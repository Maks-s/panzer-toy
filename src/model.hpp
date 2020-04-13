#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "mesh.hpp"

class Shader;

class Model {
public:
	Model() {};
	Model(const std::string& path) { load(path); };
	void load(std::string path);
	void draw(const Shader& shader, GLint uniform_MVP, const glm::mat4& VP);

	void set_pos(const glm::vec3& pos);
	void move(const glm::vec3& pos);

	void set_angle(float angle);
	void rotate(float angle);

	glm::vec3 get_pos() const { return pos; };
	float get_angle() const { return angle; };

	bool is_empty() { return meshes.empty(); };

private:
	std::vector<Mesh> meshes;
	glm::mat4 model_mat = glm::mat4(0.0f);
	glm::vec3 pos = glm::vec3(0.0f);
	float angle = 0.0f;
	bool dirty = false;
};

#endif // MODEL_HPP
