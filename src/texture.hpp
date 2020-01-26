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
	Texture loadTexture(std::string filename, bool flip = false, aiTextureType type = aiTextureType_NONE);

	Texture getTexture(std::string filename);
	Texture getTexture(GLuint id);
}

#endif // TEXTURE_HPP
