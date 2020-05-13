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
	Map(int map_id) : Map() { load(map_id, 0.0f); };
	void load(int map_id, float time);
	void draw(const Shader& shader, const glm::mat4& VP) const;
	void reset(float time);

	MapCollision collision_check(const glm::vec3& pos) const;
	glm::vec3 get_player_starting_pos() const;

	int get_map_id() const { return map_id; };

private:
	void process_object(int row, int column, MapObject type);

	// 2 dimensional array, 22 columns 16 rows
	std::array<std::array<MapObject, 22>, 16> datamap{};
	int map_id = 0;
};

#endif // MAP_HPP
