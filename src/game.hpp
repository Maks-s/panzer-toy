#ifndef GAME_HPP
#define GAME_HPP

#include <memory>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "map.hpp"
#include "player.hpp"
#include "shader.hpp"

class Game {
public:
	Game();
	~Game();
	void run();
	void close();
	void finish_level();
	void reset_level();
	void player_shoot() const;
	bool player_bullet_collision(const glm::vec3& bullet_pos) {
		return player->bullet_collision(*this, bullet_pos);
	};

	void set_ratio(float ratio) { cam.set_ratio(ratio); }
	void set_window_width(float width) { window_width = width; };
	void set_window_height(float height) { window_height = height; };

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

	Shader base_shader;
	Camera cam;
	float current_time;
	glm::vec2 cursor_pos;
	// Instancing directly will segfault because OpenGL isn't initialised
	std::unique_ptr<Map> map;
	std::unique_ptr<Player> player;
	GLint uniform_time;
	GLFWwindow* window;
	float window_width = 600.0f;
	float window_height = 500.0f;
};

#endif // GAME_HPP
