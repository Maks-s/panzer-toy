#ifndef ANIMATED_MODEL_HPP
#define ANIMATED_MODEL_HPP

#include <string>
#include <vector>
#include <assimp/scene.h>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "animatedmesh.hpp"

class Shader;

class AnimatedModel {
public:
	AnimatedModel() {};
	AnimatedModel(const std::string& path);
	virtual ~AnimatedModel() = default;

	virtual bool load(std::string path);
	virtual void draw(const Shader& shader, const glm::mat4& VP);
	void calculate_bone_mtx();

	glm::vec3 get_pos() const { return pos; };
	void set_pos(const glm::vec3& pos);
	void move(const glm::vec3& pos);

	float get_angle() const { return angle; };
	void set_angle(float angle);
	void rotate(float angle);

	void set_bone_angle(float _angle);
	float get_bone_angle() { return bone_angle; };

	bool is_empty() { return meshes.empty(); };

private:
	void load_node(const aiScene* scene, aiNode* node, const std::string& path);
	float bone_angle = 2.0f;
	glm::mat4 bone_mtx = glm::mat4(1.0f);
	glm::mat4 bone_base_mtx = glm::mat4(1.0f);

	std::vector<AnimatedMesh> meshes;
	glm::mat4 model_mat = glm::mat4(0.0f);
	glm::vec3 pos = glm::vec3(0.0f);
	float angle = 0.0f;
	bool dirty = false;
};

#endif // ANIMATED_MODEL_HPP
