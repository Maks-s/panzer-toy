#ifndef PANZERTOY_MAP_HPP
#define PANZERTOY_MAP_HPP

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
	nothing = 0, /**< nothing (air) */
	strong_wall = 1, /**< doesn't let anything through. Unbreakable. */
	weak_wall = 2, /**< doesn't let anything through, but can break */
	hole = 3, /**< block tanks but let bullets through */
	player = 4, /**< player's spawn. Maps must have only 1 */
	enemy = 5 /**< Anything higher or equal to this ID is an enemy. @see EnemyType */
};

/**
 * @class Map
 *
 * @brief Represent the maps loaded from assets/map_*.txt
 */
class Map {
public:
	Map() = default;
	Map(int map_id);
	bool load(int map_id);
	void draw(const Shader& shader, const glm::mat4& VP) const;
	void restart();

	static void init();

	MapCollision collision_check(const glm::vec3& pos) const;
	glm::vec3 get_player_starting_pos() const;

	int get_map_id() const { return map_id; };

private:
	void process_object(int row, int column, MapObject type);

	// 2D array, 22 columns 16 rows
	std::array<std::array<MapObject, 22>, 16> datamap{};
	int map_id = -1;
};

#endif // PANZERTOY_MAP_HPP
