#include <ios>
#include <system_error>
#include <memory>
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
#include "model.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "version.hpp"

// @TODO: Make everything compliant with C++ Core Guidelines
// @TODO: Document everything

static void cursor_pos_callback(GLFWwindow* window, double x, double y) {
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	game->set_cursor_pos(glm::vec2(x, y));
}

static void mouse_btn_callback(GLFWwindow* window, int btn, int action, int) {
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

	if (btn != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) {
		return;
	}

	game->player_shoot();
}

// @TODO: Add restrictions to resizing, to prevent the void
static void resize_window_callback(GLFWwindow* window, int width, int height) {
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

	game->window_resize_callback(static_cast<float>(width), static_cast<float>(height));

	glViewport(0, 0, width, height);
}

// @TODO: Make width and height integers
void Game::window_resize_callback(float width, float height) {
	width = (width == 0.0f) ? window_width : width;
	height = (height == 0.0f) ? window_height : height;

	window_width = width;
	window_height = height;

	cam.set_ratio(width / height);

	text.set_pos(glm::vec2(0.0f, window_height - 10.0f));

	glm::mat4 projection = glm::ortho(0.0f, width, height, 0.0f);

	text_settings.projection = projection;
	sprite_infos.shader.set_MVP(projection);
	Text::window_size(text_settings, width, height);
}

Game::Game() {
	if (!glfwInit()) {
		throw std::system_error(EINTR, std::generic_category(), "Error initialising GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(window_width, window_height, PANZERTOY_TITLE, nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		throw std::system_error(EINTR, std::generic_category(), "Error creating window");
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, static_cast<void*>(this));
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_btn_callback);
	glfwSetWindowSizeCallback(window, resize_window_callback);
	glfwSwapInterval(1); // @TODO: Use delta time instead of this cheap trick

	if (gl3wInit()) {
		glfwTerminate();
		throw std::system_error(EINTR, std::generic_category(), "Error initialising GL3W");
	}

	glViewport(0, 0, window_width, window_height);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Texture settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	base_shader.load("vertex.glsl", "fragment.glsl");

	Sprite::init(sprite_infos);
	Text::init_settings(text_settings);

	text.set_text("You're beautiful");
	sprite.load("assets/sprite.png");

	window_resize_callback(0.0f, 0.0f);

	// Set up the camera to be aligned with the map
	cam.set_pos(glm::vec3(11.0f, 10.0f, 10.5f));
	cam.set_angle(glm::vec2(0.0f, -1.9f));

	map = std::make_unique<Map>(0);
	player = std::make_unique<Player>(map->get_player_starting_pos());

	EnemyManager::init();
}

void Game::run() {
	while (!glfwWindowShouldClose(window)) {
		frame();
	}
}

void Game::player_shoot() const {
	player->shoot(*this);
}

float Game::calculate_cursor_angle(const glm::mat4& VP) const {
	// World coordinates to screen coordinates
	glm::vec4 world_pos = VP * glm::vec4(player->get_pos(), 1.0f);
	world_pos /= world_pos.w;
	float screen_x = (world_pos.x + 1) / 2.0f * window_width + 0.5f;
	float screen_y = (1 - world_pos.y) / 2.0f * window_height + 0.5f;

	// Set origin to player, and change to cartesian coordinate system
	float x = cursor_pos.x - screen_x;
	float y = screen_y - cursor_pos.y;

	return glm::atan(y, x) - glm::pi<float>();
}

void Game::frame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	current_time = glfwGetTime();
	base_shader.use();

	// WASD / ZQSD controls
	player->handle_movement(*this, window);

	const glm::mat4 VP = cam.get_VP();
	EnemyManager::frame(*this, base_shader, VP);
	BulletManager::frame(*this, base_shader, VP);

	player->set_top_angle(calculate_cursor_angle(VP));
	player->draw(base_shader, VP);
	map->draw(base_shader, VP);

	text.draw(text_settings);
	sprite.draw(sprite_infos);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Game::finish_level() {
	BulletManager::clear();

	try {
		map->load(map->get_map_id() + 1, current_time);
	} catch (const std::ios_base::failure) {
		Log::info("You finished the game, congratulations :D");
		close();
	} catch (const std::exception& e) {
		Log::error(e.what());
		close();
	}

	player->set_pos(map->get_player_starting_pos());
}

// @TODO: Lives and a nice Game Over screen
void Game::reset_level() {
	BulletManager::clear();
	EnemyManager::clear();
	map->reset(current_time);
	player->set_pos(map->get_player_starting_pos());
	player->set_base_angle(0.0f);
	player->set_last_shoot_time(current_time);
}

void Game::close() {
	glfwSetWindowShouldClose(window, true);
}

Game::~Game() {
	glfwTerminate();
}
