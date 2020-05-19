#ifndef PANZERTOY_TEXT_HPP
#define PANZERTOY_TEXT_HPP

#include <unordered_map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>

#include "shader.hpp"

// @TODO: Use a texture atlas to draw text
struct GlyphTexture {
	GLuint id = 0;
	float vertices[4][4] = {};
	FT_Pos advance = 0;
};

struct TextSettings {
	FT_Library lib;
	FT_Face face;
	Shader shader;
	glm::mat4 projection;
	bool uninitialized = true;
	std::unordered_map<char, GlyphTexture> glyph_list;
	GLuint VAO, VBO, EBO;
};

class Text {
public:
	Text() = default;
	Text(TextSettings& settings);
	Text(TextSettings& settings, const std::string& text) : Text(settings) { set_text(text); };

	static void init_settings(TextSettings& settings);
	static void window_size(TextSettings& settings, int width, int height);

	void draw(const TextSettings& settings);

	void set_text(const std::string& text) { this->text = text; };
	std::string get_text() const { return text; };

	void set_pos(const glm::ivec2& pos);
	glm::ivec2 get_pos() const { return pos; };

	void set_scale(float scale);
	void set_color(const glm::vec3& color) { this->color = color; };

private:
	bool dirty = true;
	std::string text;
	float scale = 1.0f;
	glm::ivec2 pos = glm::ivec2(0.0f);
	glm::vec3 color = glm::vec3(1.0f);
	glm::mat4 transform;
};

#endif // PANZERTOY_TEXT_HPP
