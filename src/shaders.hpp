#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/gl3w.h>
#include <glm/mat4x4.hpp>

#define uniformFunc(...) setUniform(GLint location, __VA_ARGS__)

class shader {
private:
	GLuint glProgram;
	bool hasFailed;

public:
	shader(const char* vtxFilename, const char* fragFilename);
	void use() const;
	bool fail() const { return hasFailed; }
	GLint getUniformLocation(const char* name) const;

	// Uniforms overload
	void uniformFunc(float) const;
	void uniformFunc(float, float, float, float) const;
	void uniformFunc(int) const;
	void uniformFunc(glm::mat4) const;
};

#endif // SHADER_HPP
