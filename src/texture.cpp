#define STB_IMAGE_IMPLEMENTATION
#include <string>
#include <vector>
#include <stb_image.h>
#include <GL/gl3w.h>
#include <assimp/material.h>

#include "error.hpp"
#include "texture.hpp"

std::vector<Texture> loaded_textures;

Texture TextureManager::load_texture(std::string filename, bool flip, aiTextureType type) {
	for (int i=loaded_textures.size() - 1; i >= 0; --i) {
		if (loaded_textures[i].path == filename) {
			return loaded_textures[i];
		}
	}

	Texture texture = {
		.path = filename,
		.type = type,
	};

	stbi_set_flip_vertically_on_load(flip);

	int width, height, comp;
	unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, &comp, STBI_default);

	if (!pixels) {
		PRINT_ERROR("Could not load texture image at " << filename);
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
		PRINT_ERROR("Unknown image components: " << filename);
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
	loaded_textures.push_back(texture);

	return texture;
}

Texture TextureManager::get_texture(std::string filename) {
	for (auto& texture : loaded_textures) {
		if (texture.path == filename) {
			return texture;
		}
	}

	return Texture(); // Return invalid texture
}

Texture TextureManager::get_texture(GLuint id) {
	for (auto& texture : loaded_textures) {
		if (texture.id == id) {
			return texture;
		}
	}

	return Texture();
}
