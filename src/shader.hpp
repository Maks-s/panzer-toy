#ifndef PANZERTOY_SHADER_HPP
#define PANZERTOY_SHADER_HPP

#include <GL/gl3w.h>
#include <glm/glm.hpp>

class Shader {
public:
	Shader() = default;
	Shader(const char* vtx_path, const char* frag_path) { load(vtx_path, frag_path); };
	void load(const char* vtx_path, const char* frag_path);
	void use() const;
	GLint get_uniform_location(const char* name) const;
	void set_MVP(const glm::mat4& MVP) const;

	// Uniforms overload
	static void set_uniform(GLint, float);
	static void set_uniform(GLint, float, float, float);
	static void set_uniform(GLint, float, float, float, float);
	static void set_uniform(GLint, int);
	static void set_uniform(GLint, const glm::mat4&);

private:
	GLint uniform_MVP = -1;
	GLuint glProgram = 0;
};

#endif // PANZERTOY_SHADER_HPP
