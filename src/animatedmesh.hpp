#ifndef ANIMATED_MESH_HPP
#define ANIMATED_MESH_HPP

#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "texture.hpp"

class Shader;

struct BoneVertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoords;
	float boneWeight;
};

class AnimatedMesh {
public:
	AnimatedMesh(
		const std::vector<BoneVertex>& vertices,
		const std::vector<GLuint>& indices,
		const std::vector<Texture>& textures
	);

	void draw(const Shader& shader) const;

private:
	GLuint VAO, VBO, EBO;
	std::vector<BoneVertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
};

#endif // ANIMATED_MESH_HPP
