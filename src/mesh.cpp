#include <GL/gl3w.h>

#include "mesh.hpp"

mesh::mesh(float* vertices, GLsizeiptr size) {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void mesh::vertexAttrib(
	GLuint index,
	GLint size,
	GLenum type,
	GLboolean normalized,
	GLsizei stride,
	const void* pointer) {

	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	glEnableVertexAttribArray(index);
}

void mesh::vertexAttrib(GLuint index, GLint size) {
	vertexAttrib(index, size, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void mesh::bind() {
	glBindVertexArray(VAO);
}
