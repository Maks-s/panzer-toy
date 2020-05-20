#include <string>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "sprite.hpp"

namespace {
	const GLuint indices[6] = {0, 1, 2, 0, 2, 3}; // Indices used to draw a rectangle
}

void Sprite::init(SpriteRenderInfos& infos) {
	glGenVertexArrays(1, &infos.VAO);
	glBindVertexArray(infos.VAO);

	glGenBuffers(1, &infos.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, infos.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Sprite::vertices), nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &infos.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, infos.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

	glBindVertexArray(0);

	infos.shader.load("vertex_2d.glsl", "fragment.glsl");
	infos.shader.use();

	// It's a sprite, so we only have one texture: GL_TEXTURE0
	Shader::set_uniform(infos.shader.get_uniform_location("textureDiffuse0"), (int)0);
}

/** @brief Load a sprite */
void Sprite::load(const std::string& filename) {
	texture = TextureManager::load_texture(filename);
	size = glm::ivec2(texture.width, texture.height);
}

bool Sprite::draw(SpriteRenderInfos& infos) {
	if (dirty) {
		calculate_vertices();
		dirty = false;
	}

	infos.shader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(infos.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, infos.VBO);

	glBindTexture(GL_TEXTURE_2D, texture.id);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	return true;
}

void Sprite::set_pos(const glm::ivec2& pos) {
	this->pos = pos;
	dirty = true;
}

/**
 * @brief Set size
 *
 * @param size If a value is 0 or less, it will keep the current one
 */
void Sprite::set_size(glm::ivec2 size) {
	if (size.x <= 0) {
		size.x = this->size.x;
	}

	if (size.y <= 0) {
		size.y = this->size.y;
	}

	this->size = size;
	dirty = true;
}

void Sprite::calculate_vertices() {
	vertices[0][0] = pos.x;
	vertices[0][1] = pos.y + size.y;
	vertices[0][2] = 0.0f;
	vertices[0][3] = 1.0f;

	vertices[1][0] = pos.x;
	vertices[1][1] = pos.y;
	vertices[1][2] = 0.0f;
	vertices[1][3] = 0.0f;

	vertices[2][0] = pos.x + size.x;
	vertices[2][1] = pos.y;
	vertices[2][2] = 1.0f;
	vertices[2][3] = 0.0f;

	vertices[3][0] = pos.x + size.x;
	vertices[3][1] = pos.y + size.y;
	vertices[3][2] = 1.0f;
	vertices[3][3] = 1.0f;
}
