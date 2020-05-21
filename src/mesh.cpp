#include <string>
#include <GL/gl3w.h>
#include <assimp/material.h>

#include "log.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"

Mesh::Mesh(
		const std::vector<Vertex>& vertices,
		const std::vector<GLuint>& indices,
		const std::vector<Texture>& textures
	) {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);

	indice_count = indices.size();
	this->textures = textures;
}

void Mesh::draw(const Shader& shader) const {
	int diffuse = 0;
	int specular = 0;

	int textures_size = textures.size();
	for (int i = 0; i < textures_size; ++i) {
		std::string name = "texture";

		switch (textures[i].type) {
		case aiTextureType_DIFFUSE:
			name += "Diffuse" + std::to_string(diffuse++);
			break;
		case aiTextureType_SPECULAR:
			name += "Specular" + std::to_string(specular++);
			break;
		default:
			continue;
		}

		glActiveTexture(GL_TEXTURE0 + i);

		Shader::set_uniform(shader.get_uniform_location(name.c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indice_count, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

/** @brief Delete bound VAO, VBO and EBO */
void Mesh::clear_gl() const {
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}
