#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "error.hpp"
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
**/

std::unique_ptr<Model> Map::strong_wall;

Map::Map(const char* filename) {
	if (!strong_wall) {
		Model mdl("models/strong_wall.obj");
		strong_wall = std::make_unique<Model>(mdl);
	}

	std::ifstream file(filename);

	if (!file.is_open()) {
		PRINT_ERROR("Error opening file: " << filename);
		failed = true;
		return;
	}

	for (int i=0; i < 16; ++i) {
		if (file.eof()) {
			PRINT_ERROR("Invalid map (not enough row): " << filename);
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
			PRINT_ERROR("Invalid map (column number mismatch): " << filename);
			failed = true;
			return;
		}

		for (int j=0; j < 22; ++j) {
			int stocked = std::stoi(data[j]);
			datamap[i][j] = stocked;
			source[i][j] = stocked;
		}
	}
}

void Map::draw(Shader shader, GLint uniform_MVP, glm::mat4 VP) {
	for (int i=0; i < 16; ++i) {
		for (int j=0; j < 22; ++j) {
			Model* mdl;

			switch (datamap[i][j]) {
			case 0:
				continue;
			case 1:
				mdl = strong_wall.get();
				break;
			default:
				PRINT_ERROR("Unimplemented map number");
				break;
			}

			mdl->set_position(glm::vec3(i, 0.0f, j));
			mdl->draw(shader, uniform_MVP, VP);
		}
	}
}
