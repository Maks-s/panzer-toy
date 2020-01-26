#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "shaders.hpp"
#include "texture.hpp"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh {
public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);

	void draw(Shader shader);

private:
	GLuint mVAO, mVBO, mEBO;
	std::vector<Vertex> mVertices;
	std::vector<GLuint> mIndices;
	std::vector<Texture> mTextures;
};

#endif // MESH_HPP
