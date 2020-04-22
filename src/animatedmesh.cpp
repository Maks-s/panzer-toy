// See header of animatedmodel.cpp
// See header of animatedmodel.cpp
// See header of animatedmodel.cpp
// See header of animatedmodel.cpp
// See header of animatedmodel.cpp
// See header of animatedmodel.cpp
// See header of animatedmodel.cpp
// See header of animatedmodel.cpp
// See header of animatedmodel.cpp

#include <string>
#include <GL/gl3w.h>
#include <assimp/material.h>

#include "animatedmesh.hpp"
#include "shader.hpp"
#include "texture.hpp"

AnimatedMesh::AnimatedMesh(
	const std::vector<BoneVertex>& vertices,
	const std::vector<GLuint>& indices,
	const std::vector<Texture>& textures
	) {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BoneVertex), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)offsetof(BoneVertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)offsetof(BoneVertex, texCoords));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)offsetof(BoneVertex, boneWeight));

	glBindVertexArray(0);

	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
}

void AnimatedMesh::draw(const Shader& shader) const {
	int diffuse = 0;
	int specular = 0;

	for (int i = textures.size() - 1; i >= 0; --i) {
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

		shader.set_uniform(shader.get_uniform_location(name.c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}
