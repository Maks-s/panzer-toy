#include <fstream>
#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "shaders.hpp"
#include "errors.hpp"

std::string readfile(const char* filename);

shader::shader(const char* vtxFilename, const char* fragFilename) {
	GLuint vtxShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vtxSource = readfile(vtxFilename);
	std::string fragSource = readfile(fragFilename);

	const char* pSource = vtxSource.data();
	glShaderSource(vtxShader, 1, &pSource, nullptr);
	glCompileShader(vtxShader);

	pSource = fragSource.data();
	glShaderSource(fragShader, 1, &pSource, nullptr);
	glCompileShader(fragShader);

	GLint vtxSuccess, fragSuccess;
	glGetShaderiv(vtxShader, GL_COMPILE_STATUS, &vtxSuccess);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &fragSuccess);

	if (!vtxSuccess) {
		char info[512];
		glGetShaderInfoLog(vtxShader, 512, nullptr, info);
		PRINT_ERROR("Error compiling vertex shader: " << std::endl << info);
	}

	if (!fragSuccess) {
		char info[512];
		glGetShaderInfoLog(fragShader, 512, nullptr, info);
		PRINT_ERROR("Error compiling fragment shader: " << std::endl << info);
	}

	has_failed = (!fragSuccess || !vtxSuccess);
	if (has_failed)
		return;

	glProgram = glCreateProgram();
	glAttachShader(glProgram, vtxShader);
	glAttachShader(glProgram, fragShader);
	glLinkProgram(glProgram);

	GLint progSuccess;
	glGetProgramiv(glProgram, GL_LINK_STATUS, &progSuccess);
	if (!progSuccess) {
		char info[512];
		glGetProgramInfoLog(glProgram, 512, nullptr, info);
		PRINT_ERROR("Error linking shaders: " << std::endl << info);
	}

	has_failed = !progSuccess;

	glDeleteShader(vtxShader);
	glDeleteShader(fragShader);
}

void shader::use() {
	glUseProgram(glProgram);
}

bool shader::fail() {
	return has_failed;
}

GLint shader::getUniformLocation(const char* name) {
	return glGetUniformLocation(glProgram, name);
}

std::string readfile(const char* filename) {
	std::ifstream shaderfile(filename, std::ios::ate);

	if (!shaderfile.is_open()) {
		PRINT_ERROR("Error opening file");
		return "";
	}

	size_t size = shaderfile.tellg();
	shaderfile.seekg(0, std::ios::beg);

	std::string content(size, '\0');
	shaderfile.read(&content[0], size);

	shaderfile.close();

	return content;
}

// If you wonder what "location" is, see uniformFunc macro in shaders.hpp
void shader::uniformFunc(float f) {
	glUniform1f(location, f);
}

void shader::uniformFunc(float x, float y, float z, float w) {
	glUniform4f(location, x, y, z, w);
}
