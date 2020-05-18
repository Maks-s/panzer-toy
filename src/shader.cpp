#include <fstream>
#include <stdexcept>
#include <string>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "log.hpp"

static std::string readfile(const char* filename);

// @TODO: Make shaders' root at shaders/
void Shader::load(const char* vtx_path, const char* frag_path) {
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
		throw std::runtime_error((std::string)"Error compiling vertex shader: \n" + info);
	}

	if (!frag_success) {
		char info[512];
		glGetShaderInfoLog(frag_shader, 512, nullptr, info);
		throw std::runtime_error((std::string)"Error compiling fragment shader: \n" + info);
	}

	glProgram = glCreateProgram();
	glAttachShader(glProgram, vtx_shader);
	glAttachShader(glProgram, frag_shader);
	glLinkProgram(glProgram);

	GLint prog_success;
	glGetProgramiv(glProgram, GL_LINK_STATUS, &prog_success);
	if (!prog_success) {
		char info[512];
		glGetProgramInfoLog(glProgram, 512, nullptr, info);
		throw std::runtime_error((std::string)"Error linking shaders: \n" + info);
	}

	glDeleteShader(vtx_shader);
	glDeleteShader(frag_shader);

	uniform_MVP = glGetUniformLocation(glProgram, "MVP");
	if (uniform_MVP == -1) {
		throw std::runtime_error((std::string)"No MVP uniform in shader: " + vtx_path);
	}
}

void Shader::use() const {
	glUseProgram(glProgram);
}

GLint Shader::get_uniform_location(const char* name) const {
	return glGetUniformLocation(glProgram, name);
}

void Shader::set_MVP(const glm::mat4& MVP) const {
	use();
	set_uniform(uniform_MVP, MVP);
}

static std::string readfile(const char* filename) {
	std::ifstream shaderfile(filename, std::ios::ate);

	if (!shaderfile.is_open()) {
		Log::error("Error opening file: ", filename);
		return "";
	}

	size_t size = shaderfile.tellg();
	shaderfile.seekg(0, std::ios::beg);

	std::string content(size, '\0');
	shaderfile.read(&content[0], size);

	shaderfile.close();

	return content;
}

void Shader::set_uniform(GLint location, float f) {
	glUniform1f(location, f);
}

void Shader::set_uniform(GLint location, float x, float y, float z) {
	glUniform3f(location, x, y, z);
}

void Shader::set_uniform(GLint location, float x, float y, float z, float w) {
	glUniform4f(location, x, y, z, w);
}

void Shader::set_uniform(GLint location, int i) {
	glUniform1i(location, i);
}

void Shader::set_uniform(GLint location, const glm::mat4& mat) {
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}
