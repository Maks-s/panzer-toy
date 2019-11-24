#ifndef MESH_HPP
#define MESH_HPP

#include <GL/gl3w.h>

class mesh {
private:
	GLuint VAO;
	GLuint VBO;

public:
	mesh(float* vertices, GLsizeiptr size);

	void vertexAttrib(GLuint index, GLint size);
	void vertexAttrib(GLuint index, GLint size, GLsizei stride);
	void vertexAttrib(GLuint index, GLint size, GLsizei stride, const void* pointer);
	void vertexAttrib(
		GLuint index,
		GLint size,
		GLenum type,
		GLboolean normalized,
		GLsizei stride,
		const void* pointer
	);

	void bind();
};

#endif // MESH_HPP
