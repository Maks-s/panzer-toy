#ifndef PANZERTOY_MESH_HPP
#define PANZERTOY_MESH_HPP

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
		const std::vector<Texture>& textures
	);

	void clear_gl() const;
	void draw(const Shader& shader) const;

private:
	GLuint VAO, VBO, EBO;
	// @TODO: See if we can delete those
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
};

#endif // PANZERTOY_MESH_HPP
