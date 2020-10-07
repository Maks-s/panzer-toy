#include <string>
#include <system_error>
#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "log.hpp"
#include "text.hpp"
#include "shader.hpp"

namespace {
	inline bool process_character(TextRenderInfos& settings, char c);
	const GLuint indices[6] = {0, 1, 2, 0, 2, 3};
}

void Text::init(TextRenderInfos& settings) {
	FT_Error error = FT_Init_FreeType(&settings.lib);

	if (error) {
		throw std::runtime_error(
			"Error initializing FreeType library: " + std::to_string(error)
		);
	}

	error = FT_New_Face(settings.lib, "assets/Square.ttf", 0, &settings.face);

	if (error) {
		throw std::runtime_error("Error creating FreeType face: " + std::to_string(error));
	}

	glGenVertexArrays(1, &settings.VAO);
	glBindVertexArray(settings.VAO);

	glGenBuffers(1, &settings.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, settings.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GlyphTexture::vertices), nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &settings.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, settings.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

	glBindVertexArray(0);

	settings.shader.load("vertex_2d.glsl", "frag_greyscale.glsl");
	settings.shader.use();
	Shader::set_uniform(settings.shader.get_uniform_location("textureText"), (int)0);
}

/**
 * @brief Called when the window is resized
 *
 * @param[in,out] settings Settings to generate the glyphs to
 * @param[in] width Window's width
 * @param[in] height Window's height
 */
void Text::window_size(TextRenderInfos& settings, unsigned int width, unsigned int height) {
	FT_Set_Char_Size(settings.face, 0, 16*64, 0, height);

	for (FT_ULong c = 32; c <= 125; ++c) {
		if (!process_character(settings, c)) {
			throw std::runtime_error("Error loading character " + std::to_string(c));
		}
	}
}

/**
 * @brief Calculate width of the text
 *
 * This is used to center it
 */
void Text::calculate_metrics() {
	if (text.empty() || settings == nullptr) {
		width = 0;
		return;
	}

	GlyphTexture glyph = settings->glyph_list.at(text[0]);
	last_glyph_width = glyph.advance;
	width = last_glyph_width;

	for (auto character = text.begin() + 1; character != text.end(); ++character) {
		GlyphTexture glyph = settings->glyph_list.at(*character);
		width += glyph.advance;
	}

	/**
	 * Also calculate height, because why not
	 * With our font, height is always the same so it isn't difficult
	 * @todo Make this universal
	 */
	height = settings->face->glyph->metrics.height * 0.01f;
}

void Text::draw() {
	if (settings == nullptr) {
		throw std::runtime_error("No render settings for text:\n" + text);
	}

	if (dirty) {
		transform = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f));
		transform = glm::scale(transform, glm::vec3(scale, scale, 1.0f));
		dirty = false;
	}

	if (text.empty()) {
		return;
	}

	settings->shader.use();
	Shader::set_uniform(settings->shader.get_uniform_location("color"), color.r, color.g, color.b);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(settings->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, settings->VBO);

	FT_Pos advance = 0;

	if (flags & TextFlags::CENTER_TEXT) {
		advance = -width / 2;
	}

	auto character = text.begin();
	GlyphTexture glyph = settings->glyph_list.at(*character);

	// Glyphs' size has been changed, this is triggered by resizing the window
	if (glyph.advance != last_glyph_width) {
		calculate_metrics();
	}

	while (true) {
		glBindTexture(GL_TEXTURE_2D, glyph.id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glyph.vertices), glyph.vertices);

		settings->shader.set_MVP(
			settings->projection * glm::translate(transform, glm::vec3(advance, 0.0f, 0.0f))
		);

		advance += glyph.advance;

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		if (++character == text.end()) {
			break;
		}

		glyph = settings->glyph_list.at(*character);
	}

	glBindVertexArray(0);
}

void Text::set_pos(const glm::ivec2& pos) {
	this->pos = pos;
	dirty = true;
}

void Text::set_scale(float scale) {
	this->scale = scale;
	calculate_metrics();
	dirty = true;
}

void Text::set_text(const std::string& text) {
	this->text = text;
	calculate_metrics();
}

namespace {
	/**
	 * @brief Load the glyph corresponding to c into settings
	 *
	 * @param[in,out] settings TextRenderInfos to load the glyph from and to put the generated bitmap
	 * @param[in] c The character to generate
	 *
	 * @return True if there was no error, false otherwise
	 *
	 * @note This function is inline because it's only called 1 time
	 */
	inline bool process_character(TextRenderInfos& settings, char c) {
		FT_Error error = FT_Load_Char(settings.face, c, FT_LOAD_RENDER);

		if (error) {
			return false;
		}

		const FT_Glyph_Metrics& metrics = settings.face->glyph->metrics;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		GLuint texture_id;

		// Use the existing texture id for this character if it exists, else generate a new one
		auto result = settings.glyph_list.find(c);
		if (result == settings.glyph_list.end()) {
			glGenTextures(1, &texture_id);
		} else {
			texture_id = result->second.id;
		}

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			settings.face->glyph->bitmap.width,
			settings.face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			settings.face->glyph->bitmap.buffer
		);

		glTextureParameteri(texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Restore unpack to default value

		const float scale = 0.01f; // Letters are BIG, so we scale them down

		GlyphTexture glyph_texture;
		glyph_texture.id = texture_id;
		glyph_texture.advance = metrics.horiAdvance * scale;

		float pos_left = metrics.horiBearingX * scale;
		float pos_right = (metrics.horiBearingX + metrics.width) * scale;
		float pos_top = -metrics.horiBearingY * scale;
		float pos_bottom = (metrics.height - metrics.horiBearingY) * scale;

		// vertices is a reference to glyph_texture.vertices
		auto vertices = glyph_texture.vertices;

		vertices[0][0] = pos_left;
		vertices[0][1] = pos_top;
		vertices[0][2] = 0.0f;
		vertices[0][3] = 0.0f;

		vertices[1][0] = pos_left;
		vertices[1][1] = pos_bottom;
		vertices[1][2] = 0.0f;
		vertices[1][3] = 1.0f;

		vertices[2][0] = pos_right;
		vertices[2][1] = pos_bottom;
		vertices[2][2] = 1.0f;
		vertices[2][3] = 1.0f;

		vertices[3][0] = pos_right;
		vertices[3][1] = pos_top;
		vertices[3][2] = 1.0f;
		vertices[3][3] = 0.0f;

		settings.glyph_list[c] = glyph_texture;

		return true;
	}
}
