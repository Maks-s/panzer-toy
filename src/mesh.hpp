#ifndef PANZERTOY_MESH_HPP
#define PANZERTOY_MESH_HPP

#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "texture.hpp"

class Shader;

struct Vertex {
	glm::vec3 pos;
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
	size_t indice_count;
	std::vector<Texture> textures;
};

#endif // PANZERTOY_MESH_HPP
