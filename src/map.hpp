#ifndef MAP_HPP
#define MAP_HPP

#include <array>
#include <memory>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

class Shader;

enum class Map_collision {
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
	Map(const char* filename) : Map() { load(filename); };
	void load(const char* filename);
	Map_collision collision_check(const glm::vec3& pos) const;
	void draw(const Shader& shader, GLint uniform_MVP, const glm::mat4& VP) const;
	glm::vec3 get_player_starting_pos() const;

	void reset() { datamap = source; };

private:
	// 2 dimensional array, 22 columns 16 rows
	std::array<std::array<int, 22>, 16> datamap{};
	std::array<std::array<int, 22>, 16> source{};
};

#endif // MAP_HPP
