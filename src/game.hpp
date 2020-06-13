#ifndef PANZERTOY_GAME_HPP
#define PANZERTOY_GAME_HPP

#include <memory>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "map.hpp"
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
	void finish_level();
	void restart_level();
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

private:
	float calculate_cursor_angle(const glm::mat4& VP) const;
	void frame();

	GLFWwindow* window;
	glm::ivec2 window_size = glm::ivec2(600, 500);

	Camera cam;
	Shader base_shader;
	Text text;
	Sprite sprite;

	TextRenderInfos text_settings;
	SpriteRenderInfos sprite_infos;

	float delta_time = 0.0f;
	float last_render_time = 0.0f;
	float last_fps_update = 0.0f;
	float current_time;
	glm::vec2 cursor_pos;

	// Instancing directly will segfault because OpenGL isn't initialised
	std::unique_ptr<Map> map;
	std::unique_ptr<Player> player;
};

#endif // PANZERTOY_GAME_HPP
