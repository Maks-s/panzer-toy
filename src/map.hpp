#ifndef MAP_HPP
#define MAP_HPP

#include <array>
#include <memory>

#include "model.hpp"
#include "shader.hpp"

class Map {
public:
	Map(const char* filename);
	void draw(Shader shader, GLint uniform_MVP, glm::mat4 VP);

	void reset() { datamap = source; };

private:
	// 2 dimensional array, 22 columns 16 rows
	std::array<std::array<int, 22>, 16> datamap;
	std::array<std::array<int, 22>, 16> source;
	bool failed;

	static std::unique_ptr<Model> strong_wall;
};

#endif // MAP_HPP
