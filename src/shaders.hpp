#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/gl3w.h>
#include <glm/mat4x4.hpp>

class Shader {
public:
	Shader(const char* vtxFilename, const char* fragFilename);
	void use() const;
	bool fail() const { return mFailed; }
	GLint getUniformLocation(const char* name) const;

	// Uniforms overload
	void setUniform(GLint, float) const;
	void setUniform(GLint, float, float, float, float) const;
	void setUniform(GLint, int) const;
	void setUniform(GLint, glm::mat4) const;

private:
	GLuint mGlProgram;
	bool mFailed;
};

#endif // SHADER_HPP
