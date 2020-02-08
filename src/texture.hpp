#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <GL/gl3w.h>
#include <assimp/material.h>

struct Texture {
	GLuint id = 0;
	std::string path;
	aiTextureType type = aiTextureType_NONE;
	int height = 0;
	int width = 0;
};

namespace TextureManager {
	Texture load_texture(std::string filename, aiTextureType type = aiTextureType_NONE, bool flip = false);

	Texture get_texture(std::string filename);
	Texture get_texture(GLuint id);
}

#endif // TEXTURE_HPP
