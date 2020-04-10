#ifndef MAP_HPP
#define MAP_HPP

#include <array>
#include <memory>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

class Model;
class Shader;

// @TODO: Safe class (unitialised arrays and ptr aren't safe)

class Map {
public:
	Map() {};
	Map(const char* filename);
	int collision_check(glm::vec3 pos);
	void draw(Shader shader, GLint uniform_MVP, glm::mat4 VP);
	glm::vec3 get_player_starting_pos();

	void reset() { datamap = source; };
	bool has_failed() { return failed; };

private:
	// 2 dimensional array, 22 columns 16 rows
	std::array<std::array<int, 22>, 16> datamap;
	std::array<std::array<int, 22>, 16> source;
	bool failed = false;

	static std::unique_ptr<Model> strong_wall;
};

#endif // MAP_HPP
