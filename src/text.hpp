#ifndef PANZERTOY_TEXT_HPP
#define PANZERTOY_TEXT_HPP

#include <unordered_map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>

#include "shader.hpp"

/**
 * @brief Glyph with the texture loaded on the GPU
 *
 * @todo Use a texture atlas to draw text
 */
struct GlyphTexture {
	GLuint id = 0;
	float vertices[4][4] = {};
	FT_Pos advance = 0;
};

struct TextRenderInfos {
	FT_Library lib;
	FT_Face face;
	Shader shader;
	glm::mat4 projection;
	std::unordered_map<char, GlyphTexture> glyph_list;
	GLuint VAO, VBO, EBO;
};

/**
 * @brief Text flags to control the behavior of the Text object
 *
 * @property TextFlags::CENTER_TEXT Center the text relative to its x pos
 */
enum TextFlags : short {
	CENTER_TEXT = 0b01
};

/**
 * @class Text
 *
 * @brief Represents a live 2D text on the screen
 */
class Text {
public:
	Text() = default;
	Text(TextRenderInfos* infos) { set_text_render(infos); };
	Text(const std::string& text) { set_text(text); };

	static void init(TextRenderInfos& settings);
	static void window_size(TextRenderInfos& settings, unsigned int width, unsigned int height);

	void draw();
	void calculate_metrics();

	short get_flags() const { return flags; };
	int get_width() const { return width; };
	int get_height() const { return height; };
	glm::vec2 get_pos() const { return pos; };
	void set_pos(const glm::ivec2& pos);

	void set_text_render(TextRenderInfos* infos) { settings = infos; };
	void set_text(const std::string& text);
	void set_scale(float scale);
	void set_color(const glm::vec3& color) { this->color = color; };
	void set_flags(short flags) { this->flags = flags; };

private:

	bool dirty = true;
	std::string text;
	float scale = 1.0f;
	short flags = 0b00;
	FT_Pos width = 0;
	FT_Pos height = 0;
	FT_Pos last_glyph_width = 0;
	TextRenderInfos* settings = nullptr;
	glm::vec3 color = glm::vec3(1.0f);
	glm::ivec2 pos = glm::ivec2(0); // @TODO: Indicate what is the pos point
	glm::mat4 transform; // Transformation matrix to apply to the text's vertices
};

#endif // PANZERTOY_TEXT_HPP
