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

// @TODO: Holes, weak walls

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

Map::Map() {
	if (!strong_wall) {
		strong_wall = std::make_unique<Model>(Model("models/strong_wall.obj"));
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
			datamap[i][j] = stocked;
			source[i][j] = stocked;

			if (stocked == 4) {
				required_ply_spawn = true;
			} else if (stocked == 4 && required_ply_spawn) {
				Log::error("Invalid map (more than one player spawn)");
				failed = true;
				return;
			}
		}
	}

	if (!required_ply_spawn) {
		Log::error("Invalid map (no player spawn)");
		failed = true;
	}
}

void Map::draw(Shader shader, GLint uniform_MVP, glm::mat4 VP) {
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
int Map::collision_check(glm::vec3 pos) {
	// Normal map bounds
	if (pos.x > 15 || pos.x < 0 || pos.z > 21 || pos.z < 0)
		return true;

	// Check if each point of the bounding box square is in a wall
	// @TODO: Make it dynamic
	int x = round(pos.z - 0.35f);
	int y = round(pos.x - 0.35f);

	if (datamap[y][x] == 1)
		return 1;

	y = round(pos.x + 0.35f);

	if (datamap[y][x] == 1)
		return 2;

	x = round(pos.z + 0.35f);

	if (datamap[y][x] == 1)
		return 3;

	y = round(pos.x - 0.35f);

	if (datamap[y][x] == 1)
		return 4;

	return 0;
}
