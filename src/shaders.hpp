#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/gl3w.h>

#define uniformFunc(...) setUniform(GLint location, __VA_ARGS__)

class shader {
private:
	GLuint glProgram;
	bool has_failed;

public:
	shader(const char* vtxFilename, const char* fragFilename);
	void use() const;
	bool fail() const;
	GLint getUniformLocation(const char* name) const;

	// Uniforms overload
	void uniformFunc(float) const;
	void uniformFunc(float, float, float, float) const;
};

#endif // SHADER_HPP
