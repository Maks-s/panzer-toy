#ifndef PANZERTOY_TEXTURE_HPP
#define PANZERTOY_TEXTURE_HPP

#include <string>
#include <GL/gl3w.h>
#include <assimp/material.h>

/** @brief Represents a Texture loaded on the GPU */
struct Texture {
	GLuint id = 0;
	aiTextureType type = aiTextureType_NONE;
	int height = 0;
	int width = 0;
};

/**
 * @brief Manage textures
 *
 * Makes sure to not load multiple times a texture already on the GPU
 */
namespace TextureManager {
	Texture load_texture(
		const std::string& filename,
		aiTextureType type = aiTextureType_NONE,
		bool flip = false
	);

	Texture get_texture(const std::string& filename);
	Texture get_texture(GLuint id);
}

#endif // PANZERTOY_TEXTURE_HPP
