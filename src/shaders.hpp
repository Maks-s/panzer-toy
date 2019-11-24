#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/gl3w.h>

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
};

#endif // SHADER_HPP
