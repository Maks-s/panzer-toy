#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>

#include "mesh.hpp"
#include "shader.hpp"

class Model {
public:
	Model(std::string path);
	void draw(Shader shader);

private:
	std::vector<Mesh> meshes;
};

#endif // MODEL_HPP
