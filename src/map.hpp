#ifndef MAP_HPP
#define MAP_HPP

#include <array>
#include <memory>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

class Model;
class Shader;

enum Map_collision {
	none = false,
	bottom_left,
	bottom_right,
	upper_left,
	upper_right,
	right_or_left,
	up_or_down
};

class Map {
public:
	Map();
	Map(const char* filename);
	Map_collision collision_check(glm::vec3 pos);
	void draw(Shader shader, GLint uniform_MVP, glm::mat4 VP);
	glm::vec3 get_player_starting_pos();

	void reset() { datamap = source; };
	bool has_failed() { return failed; };

private:
	// 2 dimensional array, 22 columns 16 rows
	std::array<std::array<int, 22>, 16> datamap{};
	std::array<std::array<int, 22>, 16> source{};
	bool failed = false;

	static std::unique_ptr<Model> strong_wall;
	static std::unique_ptr<Model> map_mdl;
};

#endif // MAP_HPP
