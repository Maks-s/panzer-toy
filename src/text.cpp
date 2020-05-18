#include <string>
#include <system_error>
#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "text.hpp"
#include "shader.hpp"

namespace {
	inline bool process_character(TextSettings& settings, char c);
	const GLuint indices[6] = {0, 1, 2, 0, 2, 3};
}

void Text::init_settings(TextSettings& settings) {
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

	settings.shader.load("glyph_vtx.glsl", "glyph_frag.glsl");
	settings.shader.use();
	settings.shader.set_uniform(settings.shader.get_uniform_location("textureText"), (int)0);

	settings.uninitialized = false;
}

void Text::window_size(TextSettings& settings, float width, float height) {
	FT_Set_Char_Size(settings.face, 0, 16*64, 0, height);

	for (FT_ULong c = 32; c <= 125; ++c) {
		if (!process_character(settings, c)) {
			throw std::runtime_error("Error loading character " + std::to_string(c));
		}
	}
}

Text::Text(TextSettings& settings) {
	if (settings.uninitialized) {
		init_settings(settings);
	}
}

void Text::draw(const TextSettings& settings) {
	if (settings.uninitialized) {
		return;
	}

	if (dirty) {
		transform = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f));
		transform = glm::scale(transform, glm::vec3(scale, scale, 1.0f));
	}

	settings.shader.use();
	settings.shader.set_uniform(settings.shader.get_uniform_location("color"), color.r, color.g, color.b);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(settings.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, settings.VBO);

	FT_Pos advance = 0;

	for (char character : text) {
		GlyphTexture glyph = settings.glyph_list.at(character);

		glBindTexture(GL_TEXTURE_2D, glyph.id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glyph.vertices), glyph.vertices);

		settings.shader.set_MVP(
			settings.projection * glm::translate(transform, glm::vec3(advance, 0.0f, 0.0f))
		);

		advance += glyph.advance;

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	glBindVertexArray(0);
}

void Text::set_pos(const glm::vec2& pos) {
	this->pos = pos;
	dirty = true;
}

void Text::set_scale(float scale) {
	this->scale = scale;
	dirty = true;
}

namespace {

inline bool process_character(TextSettings& settings, char c)  {
	FT_Error error = FT_Load_Char(settings.face, c, FT_LOAD_RENDER);

	if (error) {
		return false;
	}

	const FT_Glyph_Metrics_& metrics = settings.face->glyph->metrics;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLuint texture_id;

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

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Restaure unpack to default value

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
