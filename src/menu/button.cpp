#include <ft2build.h>
#include FT_IMAGE_H
#include <glm/glm.hpp>

#include "button.hpp"
#include "game.hpp"
#include "text.hpp"

void Button::draw(Game& game) {
	// Execute only when cursor enter or exit
	if (_is_mouse_over(game.get_cursor_pos())) {
		if (_cursor_has_entered) {
			_text.set_color(glm::vec3(1.0f, 0.0f, 0.0f));
			game.set_cursor(true);
			_cursor_has_entered = false;
		}
	} else {
		if (!_cursor_has_entered) {
			_text.set_color(glm::vec3(0.0f, 1.0f, 0.0f));
			game.set_cursor(false);
			_cursor_has_entered = true;
		}
	}

	_text.draw();
}

bool Button::_is_mouse_over(const glm::vec2& cursor_pos) const {
	const int width = _text.get_width();
	const int height = _text.get_height();
	glm::vec2 pos = _text.get_pos();

	if (_text.get_flags() & TextFlags::CENTER_TEXT) {
		pos.x -= width / 2;
	}

	return (
		cursor_pos.x > pos.x
		&& cursor_pos.x < pos.x + width
		&& cursor_pos.y < pos.y
		&& cursor_pos.y > pos.y - height
	);
}

/** @brief Call the callback if the user is clicking on the button */
void Button::process_click(Game& game) const {
	const glm::vec2 cursor_pos = game.get_cursor_pos();

	if (_callback == nullptr) {
		return;
	}

	if (_is_mouse_over(cursor_pos)) {
		_callback(game);
	}
}
