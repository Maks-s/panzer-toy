#ifndef GAME_HPP
#define GAME_HPP

#include <memory>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "map.hpp"
#include "shader.hpp"

class Player;

class Game {
public:
	Game();
	~Game();
	void run();

	void set_cursor_pos(glm::vec2 pos) { cursor_pos = pos; };
	glm::vec2 get_cursor_pos() { return cursor_pos; };
	Map get_map() { return map; };

private:
	Shader base_shader;
	Camera cam;
	glm::vec2 cursor_pos;
	Map map;
	// Instancing player directly will segfault because OpenGL isn't initialised
	std::unique_ptr<Player> player;
	GLint uniform_time;
	GLint uniform_MVP;
	GLFWwindow* window;

	void tick();
};

#endif // GAME_HPP
