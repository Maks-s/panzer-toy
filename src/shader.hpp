#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/gl3w.h>
#include <glm/glm.hpp>

class Shader {
public:
	Shader() {};
	Shader(const char* vtx_path, const char* frag_path);
	void use() const;
	bool fail() const { return failed; }
	GLint get_uniform_location(const char* name) const;

	// Uniforms overload
	void set_uniform(GLint, float) const;
	void set_uniform(GLint, float, float, float, float) const;
	void set_uniform(GLint, int) const;
	void set_uniform(GLint, glm::mat4) const;

private:
	GLuint glProgram = 0;
	bool failed = false;
};

#endif // SHADER_HPP
