#include <ios>
#include <memory>
#include <string>
#include <system_error>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game.hpp"
#include "bullet.hpp"
#include "camera.hpp"
#include "enemy.hpp"
#include "log.hpp"
#include "map.hpp"
#include "menu/button.hpp"
#include "model.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "version.hpp"

// @TODO: Make events

namespace {
	void cursor_pos_callback(GLFWwindow* window, double x, double y) {
		Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
		game->set_cursor_pos(glm::vec2(x, y));
	}

	void mouse_btn_callback(GLFWwindow* window, int btn, int action, int) {
		Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

		if (btn != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) {
			return;
		}

		game->player_shoot();
		game->process_click();
	}

	// @TODO: Add restrictions to resizing, to prevent the void
	void resize_window_callback(GLFWwindow* window, int width, int height) {
		Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

		game->window_resize_callback(width, height);

		glViewport(0, 0, width, height);
	}

	void callback_btn(Game& game) {
		game.restart_level();
	}
}

void Game::window_resize_callback(unsigned int width, unsigned int height) {
	float f_width = static_cast<float>(width);
	float f_height = static_cast<float>(height);

	window_size = glm::uvec2(width, height);

	// Projection is screen
	glm::mat4 projection = glm::ortho(0.0f, f_width, f_height, 0.0f);

	text_render.projection = projection;
	sprite_render.shader.set_MVP(projection);
	Text::window_size(text_render, width, height);

	cam.set_ratio(f_width / f_height);

	btn.calculate_metrics();
	btn.set_pos(glm::ivec2(width / 2, 10 + btn.get_height()));
	fps_counter.set_pos(glm::ivec2(width / 2, height - 10));
}

/**
 * @brief Initialize everything that needs to be initialized
 *
 * Call all initializers of classes or namespaces
 *
 * @exception system_error::EINTR Error while initializing required components
 */
Game::Game() {
	if (!glfwInit()) {
		throw std::system_error(EINTR, std::generic_category(), "Error initializing GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(window_size.x, window_size.y, PANZERTOY_TITLE, nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		throw std::system_error(EINTR, std::generic_category(), "Error creating window");
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, static_cast<void*>(this));
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_btn_callback);
	glfwSetWindowSizeCallback(window, resize_window_callback);
	glfwSwapInterval(0); // Disable VSync for accurate FPS

	if (gl3wInit()) {
		glfwTerminate();
		throw std::system_error(EINTR, std::generic_category(), "Error initialising GL3W");
	}

	glViewport(0, 0, window_size.x, window_size.y);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Texture settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	base_shader.load("vertex.glsl", "fragment.glsl");

	EnemyManager::init();
	Map::init();
	Sprite::init(sprite_render);
	Tank::init();
	Text::init(text_render);

	cursor_normal = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	cursor_clicker = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

	// Set up the camera to be aligned with the map
	cam.set_pos(glm::vec3(11.0f, 10.0f, 10.5f));
	cam.set_angle(glm::vec2(0.0f, -1.9f));

	fps_counter.set_text_render(&text_render);
	fps_counter.set_flags(TextFlags::CENTER_TEXT);

	btn.set_text("Restart");
	btn.set_flags(TextFlags::CENTER_TEXT);
	btn.set_render(&text_render);
	btn.set_callback(&callback_btn);

	window_resize_callback(window_size.x, window_size.y);

	map = std::make_unique<Map>();
	player = std::make_unique<Player>();
}

/** @brief Start main game loop */
void Game::run() {
	while (!glfwWindowShouldClose(window)) {
		frame();
	}
}

/** @brief Make the player shoot a bullet */
void Game::player_shoot() const {
	player->shoot(*this);
}

void Game::process_click() {
	btn.process_click(*this);
}

void Game::set_cursor(bool is_clicker) {
	if (is_clicker) {
		glfwSetCursor(window, cursor_clicker);
	} else {
		glfwSetCursor(window, cursor_normal);
	}
}

/**
 * @brief Calculate cursor angle relative to player's position
 *
 * @param VP Camera's VP to calculate player's screen position
 *
 * @return Angle of the cursor relative to the player's position on the screen
 */
float Game::calculate_cursor_angle(const glm::mat4& VP) const {
	// World coordinates to screen coordinates
	glm::vec4 world_pos = VP * glm::vec4(player->get_pos(), 1.0f);
	world_pos /= world_pos.w;
	float screen_x = (world_pos.x + 1) / 2.0f * window_size.x + 0.5f;
	float screen_y = (1 - world_pos.y) / 2.0f * window_size.y + 0.5f;

	// Set origin to player, and change to cartesian coordinate system
	float x = cursor_pos.x - screen_x;
	float y = screen_y - cursor_pos.y;

	return glm::atan(y, x) - glm::pi<float>();
}

void Game::frame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	current_time = glfwGetTime();

	delta_time = current_time - last_render_time;
	if (delta_time > 0.0833333f) {
		// We limit delta_time to 1/12 of a second, otherwise bullets could go
		// through walls, and I'm too lazy to implement Continuous Collision Detection
		delta_time = 0.0833333f;
	}

	base_shader.use();

	if (map->get_map_id() == -1) { // No map loaded but playing
		load_level(0);
	}

	// WASD / ZQSD controls
	player->handle_movement(*this, window);

	const glm::mat4 VP = cam.get_VP();
	EnemyManager::frame(*this, base_shader, VP);
	BulletManager::frame(*this, base_shader, VP);

	player->set_top_angle(calculate_cursor_angle(VP));
	player->draw(base_shader, VP);
	map->draw(base_shader, VP);

	if (current_time - last_fps_update > 1.0f) {
		float fps = 1.0f / (current_time - last_render_time);
		fps_counter.set_text("FPS: " + std::to_string(fps));
		last_fps_update = current_time;
	}

	fps_counter.draw();
	btn.draw(*this);

	last_render_time = current_time;

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Game::load_level(int id) {
	map->load(id);
	restart_level(); // Reset everything, just to be sure
}

/** @brief Finish level and switch to the next one */
void Game::finish_level() {
	BulletManager::clear();

	if (map->load(map->get_map_id() + 1)) {
		EnemyManager::set_global_last_shoot_time(current_time);
	} else {
		Log::info("You finished the game, congratulations :D");
		close();
	}

	player->set_pos(map->get_player_starting_pos());
}

/**
 * @brief Restart the current level
 *
 * @todo Lives and a nice Game Over screen
 */
void Game::restart_level() {
	const int map_id = map->get_map_id();

	if (map_id == -1) {
		Log::error("Trying to restart level but there's no map");
		return;
	}

	BulletManager::clear();
	EnemyManager::clear();
	map->restart();
	player->set_pos(map->get_player_starting_pos());
	player->set_base_angle(0.0f);
	player->set_last_shoot_time(current_time);
	EnemyManager::set_global_last_shoot_time(current_time);
}

void Game::close() {
	glfwSetWindowShouldClose(window, true);
}

Game::~Game() {
	glfwTerminate();
}
