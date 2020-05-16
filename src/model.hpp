#ifndef PANZERTOY_MODEL_HPP
#define PANZERTOY_MODEL_HPP

#include <memory>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "mesh.hpp"

class Shader;

class Model {
public:
	Model() = default;
	Model(const std::string& path);
	virtual ~Model();

	virtual bool load(std::string path);
	virtual void draw(const Shader& shader, const glm::mat4& VP);

	glm::vec3 get_pos() const { return pos; };
	void set_pos(const glm::vec3& pos);
	void move(const glm::vec3& pos);

	float get_angle() const { return angle; };
	void set_angle(float angle);
	void rotate(float angle);

	bool is_empty() const { return !meshes || meshes->empty(); };

private:
	std::string path;
	std::shared_ptr<std::vector<Mesh>> meshes;
	glm::mat4 model_mat = glm::mat4(0.0f);
	glm::vec3 pos = glm::vec3(0.0f);
	float angle = 0.0f;
	bool dirty = false;
};

#endif // PANZERTOY_MODEL_HPP
