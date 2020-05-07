#ifndef MAP_HPP
#define MAP_HPP

#include <array>
#include <memory>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

class Shader;

enum class MapCollision {
	none = false,
	bottom_left,
	bottom_right,
	upper_left,
	upper_right,
	right_or_left,
	up_or_down
};

enum class MapObject : int {
	nothing = 0,
	strong_wall = 1,
	weak_wall = 2,
	hole = 3,
	player = 4,
	enemy = 5
};

class Map {
public:
	Map();
	Map(const char* filename) : Map() { load(filename); };
	void load(const std::string& filename);
	void reset();

	MapCollision collision_check(const glm::vec3& pos) const;
	void draw(const Shader& shader, const glm::mat4& VP) const;
	glm::vec3 get_player_starting_pos() const;

private:
	void process_object(int row, int column, MapObject type);

	// 2 dimensional array, 22 columns 16 rows
	std::array<std::array<MapObject, 22>, 16> datamap{};
};

#endif // MAP_HPP
