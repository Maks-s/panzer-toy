#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <GL/gl3w.h>

#include "errors.hpp"
#include "texture.hpp"

texture::texture(const char* filename, bool flip) {
	stbi_set_flip_vertically_on_load(flip);

	unsigned char* pixels = stbi_load(filename, &width, &height, nullptr, 0);

	if (!pixels) {
		PRINT_ERROR("Could not load texture image");
		hasFailed = true;
		return;
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(pixels);
}

void texture::bind() {
	glBindTexture(GL_TEXTURE_2D, textureID);
}
