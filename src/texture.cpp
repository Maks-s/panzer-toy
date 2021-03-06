#define STB_IMAGE_IMPLEMENTATION
#include <string>
#include <unordered_map>
#include <vector>
#include <stb_image.h>
#include <GL/gl3w.h>
#include <assimp/material.h>

#include "log.hpp"
#include "texture.hpp"

namespace {
	std::unordered_map<std::string, Texture> loaded_textures;
}

Texture TextureManager::load_texture(const std::string& filename, aiTextureType type, bool flip) {
	Texture texture = get_texture(filename);
	if (texture.id != 0) {
		return texture;
	}

	texture.type = type;

	stbi_set_flip_vertically_on_load(flip);

	int width, height, comp;
	unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, &comp, STBI_default);

	if (!pixels) {
		Log::error("Could not load texture image at ", filename);
		return texture;
	}

	GLenum format;
	switch (comp) {
	case STBI_grey:
		format = GL_RED;
		break;
	case STBI_grey_alpha:
		format = GL_RG;
		break;
	case STBI_rgb:
		format = GL_RGB;
		break;
	case STBI_rgb_alpha:
		format = GL_RGBA;
		break;
	default:
		Log::error("Unknown image components: ", filename);
		return texture;
	}

	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(pixels);

	texture.id = id;
	texture.width = width;
	texture.height = height;
	loaded_textures.emplace(filename, texture);

	return texture;
}

Texture TextureManager::get_texture(const std::string& filename) {
	auto result = loaded_textures.find(filename);
	if (result != loaded_textures.end()) {
		return result->second;
	}

	return Texture{}; // Invalid texture
}
