#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>

#include "mesh.hpp"
#include "shaders.hpp"

class Model {
public:
	Model(std::string path);
	void draw(Shader shader);

private:
	std::vector<Mesh> mMeshes;
};

#endif // MODEL_HPP
