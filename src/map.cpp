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
 * WARNING:
 * The map array is accessed with map[y][x], NOT map[x][y]
 *
 * TODO:
 * Can't we change that ?
 */

namespace {
	Model strong_wall;
	Model map_mdl;
}

void Map::init() {
	strong_wall.load("models/strong_wall.dae");

	map_mdl.load("models/map.dae");
	map_mdl.set_pos(glm::vec3(7.55f, -0.0f, 10.5f));
}

Map::Map(int map_id) {
	if (!load(map_id)) {
		throw std::runtime_error("Error constructing Map");
	}
}

void Map::process_object(int x, int y, MapObject type) {
	if (static_cast<int>(type) >= 5) {
		EnemyManager::create(glm::vec3(x, 0.0f, y), static_cast<EnemyType>(type));
	}
}

/**
 * @brief Load a map file
 *
 * The map file is located in assets/map_<map_id>.txt
 *
 * @todo Throwing should load menu
 */
bool Map::load(int map_id) {
	std::string filename = "assets/map_" + std::to_string(map_id) + ".txt";

	std::ifstream file(filename);

	if (!file.is_open()) {
		Log::error("Could not open map file: ", filename);
		return false;
	}

	bool required_ply_spawn = false;

	for (int i=0; i < 16; ++i) {
		if (file.eof()) {
			Log::error("Invalid map (not enough row): ", filename);
			return false;
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
			return false;
		}

		for (int j=0; j < 22; ++j) {
			MapObject stocked = static_cast<MapObject>(std::stoi(data[j]));
			datamap[i][21 - j] = stocked;

			process_object(i, 21 - j, stocked);

			if (stocked == MapObject::player) {
				if (required_ply_spawn) {
					Log::error("Invalid map (more than one player spawn): ", filename);
					return false;
				}

				required_ply_spawn = true;
			}
		}
	}

	if (!required_ply_spawn) {
		Log::error("Invalid map (no player spawn): ", filename);
		return false;
	}

	if (EnemyManager::empty()) {
		Log::error("Invalid map (no enemy): ", filename);
		return false;
	}

	this->map_id = map_id;

	return true;
}

/** @brief Restart the currently loaded map */
void Map::restart() {
	for (int i=0; i < 16; ++i) {
		for (int j=0; j < 22; ++j) {
			process_object(i, j, datamap[i][j]);
		}
	}
}

// @TODO: Use instancing
void Map::draw(const Shader& shader, const glm::mat4& VP) const {
	map_mdl.draw(shader, VP);

	for (int i=0; i < 16; ++i) {
		for (int j=0; j < 22; ++j) {
			Model* mdl;

			if (datamap[i][j] == MapObject::strong_wall) {
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
			if (datamap[i][j] == MapObject::player) {
				return glm::vec3(i, 0.0f, j);
			}
		}
	}

	Log::error("How the hell did you get here ? Stay back, wizard !");
	return glm::vec3(0.0f);
}

/**
 * @brief Check if there was a collision
 *
 * @return MapCollision what bounding box's point collisionned with the map, or none
 *
 * @todo Make it dynamic
 * @note In "screen coordinates", x is y and z is x
 */
MapCollision Map::collision_check(const glm::vec3& pos) const {
	// Outer map bounds
	if (pos.x > 15.0f || pos.x < -0.25f) {
		return MapCollision::up_or_down;
	} else if (pos.z > 21.2f || pos.z < -0.1f) {
		return MapCollision::right_or_left;
	}

	// Check if each point of the bounding box is in a wall
	int x = round(pos.z - 0.35f);
	int y = round(pos.x - 0.35f);

	if (datamap[y][x] == MapObject::strong_wall)
		return MapCollision::upper_left;

	y = round(pos.x + 0.35f);

	if (datamap[y][x] == MapObject::strong_wall)
		return MapCollision::bottom_left;

	x = round(pos.z + 0.35f);

	if (datamap[y][x] == MapObject::strong_wall)
		return MapCollision::bottom_right;

	y = round(pos.x - 0.35f);

	if (datamap[y][x] == MapObject::strong_wall)
		return MapCollision::upper_right;

	return MapCollision::none;
}
