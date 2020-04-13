#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "texture.hpp"

class Shader;

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh {
public:
	Mesh(
		const std::vector<Vertex>& vertices,
		const std::vector<GLuint>& indices,
		const std::vector<Texture>& textures);

	void draw(const Shader& shader) const;

private:
	GLuint VAO, VBO, EBO;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
};

#endif // MESH_HPP
