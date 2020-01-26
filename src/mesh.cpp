#include <GL/gl3w.h>
#include <assimp/material.h>

#include "errors.hpp"
#include "mesh.hpp"
#include "shaders.hpp"
#include "texture.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures) {
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &mEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);

	mVertices = vertices;
	mIndices = indices;
	mTextures = textures;
}

void Mesh::draw(Shader shader) {
	int nDiffuse = 0;
	int nSpecular = 0;

	for (int i = mTextures.size() - 1; i >= 0; --i) {
		std::string name = "texture_";

		switch (mTextures[i].type) {
		case aiTextureType_DIFFUSE:
			name += "diffuse" + std::to_string(nDiffuse++);
			break;
		case aiTextureType_SPECULAR:
			name += "specular" + std::to_string(nSpecular++);
			break;
		default:
			continue;
		}

		glActiveTexture(GL_TEXTURE0 + i);

		shader.setUniform(shader.getUniformLocation(name.c_str()), i);
		glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
	}

	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}
