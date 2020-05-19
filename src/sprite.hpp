#ifndef PANZERTOY_SPRITE_HPP
#define PANZERTOY_SPRITE_HPP

#include <string>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "texture.hpp"

struct SpriteRenderInfo {
	bool uninitialized = true;
	Shader shader;
	GLuint VAO, VBO, EBO;
};

class Sprite {
public:
	Sprite() = default;
	Sprite(const std::string& filename) { load(filename); };

	static void init(SpriteRenderInfo& infos);

	void load(const std::string& filename);
	bool draw(SpriteRenderInfo& infos);

	void set_pos(const glm::ivec2& pos);
	void set_size(glm::ivec2 size);

private:
	float vertices[4][4] = {};
	bool dirty = true;
	Texture texture;
	glm::ivec2 pos = glm::ivec2(0.0f);
	glm::ivec2 size;

	void calculate_vertices();
};

#endif // PANZERTOY_SPRITE_HPP
