#include <fstream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>

#include "log.hpp"
#include "map.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "enemy.hpp"

/**
 * Map values :
 * 0 -> nothing (air)
 * 1 -> strong wall (unbreakable)
 * 2 -> weak wall (breakable)
 * 3 -> hole (diggy diggy hole)
 * 4 -> player
 * 5 -> enemy lvl 1
 * 5+i -> enemy lvl 1+i
 *
 * WARNING:
 * The map array is accessed with map[y][x], NOT map[x][y]
**/

enum class Map_object : int {
	nothing = 0,
	strong_wall = 1,
	weak_wall = 2,
	hole = 3,
	player = 4,
	enemy = 5
};

namespace {
	Model strong_wall;
	Model map_mdl;
}

Map::Map() {
	if (strong_wall.is_empty()) {
		strong_wall.load("models/strong_wall.obj");
	}

	if (map_mdl.is_empty()) {
		map_mdl.load("models/map.obj");
		map_mdl.set_pos(glm::vec3(7.55f, -0.0f, 10.5f));
		map_mdl.set_angle(0.0f);
	}
}

void Map::load(const char* filename) {
	std::ifstream file(filename);

	if (!file.is_open())
		throw std::fstream::failure((std::string)"Could not open map file" + filename);

	bool required_ply_spawn = false;

	for (int i=0; i < 16; ++i) {
		if (file.eof())
			throw std::runtime_error((std::string)"Invalid map (not enough row): " + filename);

		std::string line;
		std::getline(file, line);

		std::istringstream iss(line);
		std::vector<std::string> data(
			(std::istream_iterator<std::string>(iss)),
			std::istream_iterator<std::string>()
		);

		if (data.size() != 22)
			throw std::runtime_error((std::string)"Invalid map (column number mismatch):" + filename);

		for (int j=0; j < 22; ++j) {
			int stocked = std::stoi(data[j]);
			datamap[i][21 - j] = stocked;
			source[i][21 - j] = stocked;

			switch ((Map_object)stocked) {
			case Map_object::enemy:
				EnemyManager::create(glm::vec3(i, 0.0f, 21-j));
				break;
			case Map_object::player:
				if (required_ply_spawn)
					throw std::runtime_error((std::string)"Invalid map (more than one player spawn)" + filename);

				required_ply_spawn = true;
				break;
			default:
				break;
			}
		}
	}

	if (!required_ply_spawn)
		throw std::runtime_error((std::string)"Invalid map (no player spawn)" + filename);
}

void Map::draw(const Shader& shader, const glm::mat4& VP) const {
	map_mdl.draw(shader, VP);

	for (int i=0; i < 16; ++i) {
		for (int j=0; j < 22; ++j) {
			Model* mdl;

			if ((Map_object)datamap[i][j] == Map_object::strong_wall) {
				mdl = &strong_wall;
			} else {
				continue;
			}

			mdl->set_pos(glm::vec3(i, 0.0f, j));
			mdl->draw(shader, VP);
		}
	}
}

glm::vec3 Map::get_player_starting_pos() const {
	for (int i=0; i < 16; ++i) {
		for (int j=0; j < 22; ++j) {
			if ((Map_object)datamap[i][j] == Map_object::player) {
				return glm::vec3(i, 0.0f, j);
			}
		}
	}

	Log::error("How the hell did you get here ? Stay back, wizard !");
	return glm::vec3(0.0f);
}

// In "screen coordinates", x is y and z is x
Map_collision Map::collision_check(const glm::vec3& pos) const {
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
