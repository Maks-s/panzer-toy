#ifndef PANZERTOY_BUTTON_HPP
#define PANZERTOY_BUTTON_HPP

#include <string>
#include <glm/glm.hpp>

#include "text.hpp"

class Game;

typedef void (*BtnCallback)(Game& game);

/**
 * @class Button
 *
 * @brief Represents a clickable Text object
 * @see Text
 * @todo Add Sprite buttons (this is the reason why Button doesn't inherit from Text)
 */
class Button {
public:
	Button() { _text.set_color(glm::vec3(0.0f, 1.0f, 0.0f)); };
	Button(TextRenderInfos* render) { set_render(render); };
	Button(TextRenderInfos* render, const std::string& text) : Button(render) { set_text(text); };
	Button(
		TextRenderInfos* render,
		const std::string& text,
		const glm::ivec2& pos
	) : Button(render, text) {
		set_pos(pos);
	};

	void draw(Game& game);
	void process_click(Game& game) const;
	void calculate_metrics() { _text.calculate_metrics(); };

	int get_height() const { return _text.get_height(); };
	int get_width() const { return _text.get_width(); };

	void set_callback(BtnCallback fun) { _callback = fun; };
	void set_pos(const glm::ivec2& pos) { _text.set_pos(pos); };
	void set_flags(short flags) { _text.set_flags(flags); };
	void set_render(TextRenderInfos* render) { _text.set_text_render(render); };
	void set_text(const std::string& text) { _text.set_text(text); };

private:
	bool _is_mouse_over(const glm::vec2& cursor_pos) const;

	bool _cursor_has_entered = true;

	BtnCallback _callback = nullptr;
	Text _text;
};

#endif // PANZERTOY_BUTTON_HPP
