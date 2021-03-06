#ifndef PANZERTOY_GAME_HPP
#define PANZERTOY_GAME_HPP

#include <memory>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "map.hpp"
#include "menu/button.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "sprite.hpp"
#include "text.hpp"

/**
 * @class Game
 *
 * @brief I want to play a game.
 */
class Game {
public:
	Game();
	~Game();
	void run();
	void close();
	void load_level(int id);
	void restart_level();
	void finish_level();
	void player_shoot() const;
	bool player_bullet_collision(const glm::vec3& bullet_pos) {
		return player->bullet_collision(*this, bullet_pos);
	};

	float get_delta_time() const { return delta_time; };

	void window_resize_callback(unsigned int width, unsigned int height);
	glm::ivec2 get_window_size() const { return window_size; };

	void set_cursor_pos(glm::vec2 pos) { cursor_pos = pos; };
	glm::vec2 get_cursor_pos() const { return cursor_pos; };

	MapCollision collision_check(const glm::vec3& pos) const {
		return map->collision_check(pos);
	};

	glm::vec3 get_player_pos() const { return player->get_pos(); };
	float get_current_time() const { return current_time; };

	void process_click();
	void set_cursor(bool is_clicker);

private:
	float calculate_cursor_angle(const glm::mat4& VP) const;
	void frame();

	GLFWwindow* window;
	glm::uvec2 window_size = glm::uvec2(600, 500);

	Button btn;
	Camera cam;
	Shader base_shader;
	Text fps_counter;

	TextRenderInfos text_render;
	SpriteRenderInfos sprite_render;

	float delta_time = 0.0f;
	float last_render_time = 0.0f;
	float last_fps_update = 0.0f;
	float current_time;

	glm::vec2 cursor_pos;
	GLFWcursor* cursor_normal;
	GLFWcursor* cursor_clicker;

	// Instancing directly will segfault because OpenGL isn't initialised
	std::unique_ptr<Map> map;
	std::unique_ptr<Player> player;
};

#endif // PANZERTOY_GAME_HPP
