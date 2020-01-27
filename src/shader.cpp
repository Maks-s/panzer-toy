#include <fstream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "error.hpp"

std::string readfile(const char* filename);

Shader::Shader(const char* vtx_path, const char* frag_path) {
	GLuint vtx_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vtx_src = readfile(vtx_path);
	std::string frag_src = readfile(frag_path);

	const char* ptr_src = vtx_src.data();
	glShaderSource(vtx_shader, 1, &ptr_src, nullptr);
	glCompileShader(vtx_shader);

	ptr_src = frag_src.data();
	glShaderSource(frag_shader, 1, &ptr_src, nullptr);
	glCompileShader(frag_shader);

	GLint vtx_success, frag_success;
	glGetShaderiv(vtx_shader, GL_COMPILE_STATUS, &vtx_success);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &frag_success);

	if (!vtx_success) {
		char info[512];
		glGetShaderInfoLog(vtx_shader, 512, nullptr, info);
		PRINT_ERROR("Error compiling vertex shader: " << std::endl << info);
	}

	if (!frag_success) {
		char info[512];
		glGetShaderInfoLog(frag_shader, 512, nullptr, info);
		PRINT_ERROR("Error compiling fragment shader: " << std::endl << info);
	}

	failed = (!frag_success || !vtx_success);
	if (failed)
		return;

	glProgram = glCreateProgram();
	glAttachShader(glProgram, vtx_shader);
	glAttachShader(glProgram, frag_shader);
	glLinkProgram(glProgram);

	GLint prog_success;
	glGetProgramiv(glProgram, GL_LINK_STATUS, &prog_success);
	if (!prog_success) {
		char info[512];
		glGetProgramInfoLog(glProgram, 512, nullptr, info);
		PRINT_ERROR("Error linking shaders: " << std::endl << info);
	}

	failed = !prog_success;

	glDeleteShader(vtx_shader);
	glDeleteShader(frag_shader);
}

void Shader::use() const {
	glUseProgram(glProgram);
}

GLint Shader::get_uniform_location(const char* name) const {
	return glGetUniformLocation(glProgram, name);
}

std::string readfile(const char* filename) {
	std::ifstream shaderfile(filename, std::ios::ate);

	if (!shaderfile.is_open()) {
		PRINT_ERROR("Error opening file: " << filename);
		return "";
	}

	size_t size = shaderfile.tellg();
	shaderfile.seekg(0, std::ios::beg);

	std::string content(size, '\0');
	shaderfile.read(&content[0], size);

	shaderfile.close();

	return content;
}

void Shader::set_uniform(GLint location, float f) const {
	glUniform1f(location, f);
}

void Shader::set_uniform(GLint location, float x, float y, float z, float w) const {
	glUniform4f(location, x, y, z, w);
}

void Shader::set_uniform(GLint location, int i) const {
	glUniform1i(location, i);
}

void Shader::set_uniform(GLint location, glm::mat4 mat) const {
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}