#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>

#include "log.hpp"
#include "map.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "tank.hpp"

// @TODO: Holes, weak walls
// @TODO: Enum instead of int

/**
 * Map values :
 * 0 -> nothing (air)
 * 1 -> strong wall (unbreakable)
 * 2 -> weak wall (breakable)
 * 3 -> hole (diggy diggy hole) (will use stencils)
 * 4 -> player
 * 5 -> enemy lvl 1
 * 5+i -> enemy lvl 1+i
 *
 * WARNING:
 * The map array is accessed with map[y][x], NOT map[x][y]
**/

std::unique_ptr<Model> Map::strong_wall;
std::unique_ptr<Model> Map::map_mdl;

Map::Map() {
	if (!strong_wall) {
		strong_wall = std::make_unique<Model>("models/strong_wall.obj");
	}

	if (!map_mdl) {
		map_mdl = std::make_unique<Model>("models/map.obj");
		map_mdl->set_position(glm::vec3(7.55f, -0.0f, 10.5f));
		map_mdl->set_angle(0.0f);
	}
}

Map::Map(const char* filename) : Map::Map() {
	std::ifstream file(filename);

	if (!file.is_open()) {
		Log::error("Error opening file: ", filename);
		failed = true;
		return;
	}

	bool required_ply_spawn = false;

	for (int i=0; i < 16; ++i) {
		if (file.eof()) {
			Log::error("Invalid map (not enough row): ", filename);
			failed = true;
			return;
		}

		std::string line;
		std::getline(file, line);

		std::istringstream iss(line);
		std::vector<std::string> data(
			(std::istream_iterator<std::string>(iss)),
			std::istream_iterator<std::string>()
		);

		if (data.size() != 22) {
			Log::error("Invalid map (column number mismatch): ", filename);
			failed = true;
			return;
		}

		for (int j=0; j < 22; ++j) {
			int stocked = std::stoi(data[j]);
			datamap[i][21 - j] = stocked;
			source[i][21 - j] = stocked;

			if (stocked >= 5) {
				TankManager::create(glm::vec3(i, 0.0f, 21-j));
				continue;
			}

			if (stocked == 4) {
				if (required_ply_spawn) {
					Log::error("Invalid map (more than one player spawn)");
					failed = true;
					return;
				}

				required_ply_spawn = true;
			}
		}
	}

	if (!required_ply_spawn) {
		Log::error("Invalid map (no player spawn)");
		failed = true;
	}
}

void Map::draw(Shader shader, GLint uniform_MVP, glm::mat4 VP) {
	map_mdl->draw(shader, uniform_MVP, VP);

	for (int i=0; i < 16; ++i) {
		for (int j=0; j < 22; ++j) {
			Model* mdl;

			if (datamap[i][j] == 1) {
				mdl = strong_wall.get();
			} else {
				continue;
			}

			mdl->set_position(glm::vec3(i, 0.0f, j));
			mdl->draw(shader, uniform_MVP, VP);
		}
	}
}

glm::vec3 Map::get_player_starting_pos() {
	for (int i=0; i < 16; ++i) {
		for (int j=0; j < 22; ++j) {
			if (datamap[i][j] == 4) {
				return glm::vec3(i, 0.0f, j);
			}
		}
	}

	Log::error("How the hell did you get here ? Stay back, wizard !");
	return glm::vec3(0.0f);
}

// In "screen coordinates", x is y and z is x
Map_collision Map::collision_check(glm::vec3 pos) {
	// Normal map bounds
	if (pos.x > 15.0f || pos.x < -0.25f) {
		return Map_collision::up_or_down;
	} else if (pos.z > 21.2f || pos.z < -0.1f) {
		return Map_collision::right_or_left;
	}

	// Check if each point of the bounding box square is in a wall
	// @TODO: Make it dynamic
	int x = round(pos.z - 0.35f);
	int y = round(pos.x - 0.35f);

	if (datamap[y][x] == 1)
		return Map_collision::upper_left;

	y = round(pos.x + 0.35f);

	if (datamap[y][x] == 1)
		return Map_collision::bottom_left;

	x = round(pos.z + 0.35f);

	if (datamap[y][x] == 1)
		return Map_collision::bottom_right;

	y = round(pos.x - 0.35f);

	if (datamap[y][x] == 1)
		return Map_collision::upper_right;

	return Map_collision::none;
}
