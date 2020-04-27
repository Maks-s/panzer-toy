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
	void player_shoot() const;

	void set_cursor_pos(glm::vec2 pos) { cursor_pos = pos; };
	glm::vec2 get_cursor_pos() const { return cursor_pos; };

	MapCollision collision_check(const glm::vec3& pos) const {
		return map->collision_check(pos);
	};

	glm::vec3 get_player_pos() const { return player->get_pos(); };
	float get_current_time() const { return current_time; };

private:
	Shader base_shader;
	Shader bone_shader;
	Camera cam;
	float current_time;
	glm::vec2 cursor_pos;
	// Instancing directly will segfault because OpenGL isn't initialised
	std::unique_ptr<Map> map;
	std::unique_ptr<Player> player;
	GLint uniform_time;
	GLint uniform_bone_time;
	GLFWwindow* window;

	void frame();
};

#endif // GAME_HPP
