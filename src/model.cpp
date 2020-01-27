#include <string>
#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "error.hpp"
#include "model.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"

// Meshes loading can be changed to a parent-child system, but we don't need it right now

void load_textures(std::vector<Texture>& textures, std::string path, aiMaterial* mat, aiTextureType type);
Mesh process_mesh(aiMesh* mesh, const aiScene* scene, std::string path);

Model::Model(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		PRINT_ERROR("Error opening model, " << importer.GetErrorString());
		return;
	}

	std::string dir = path.substr(0, path.find_last_of('/') + 1);

	for (int i = scene->mNumMeshes - 1; i >= 0; --i) {
		meshes.push_back(process_mesh(scene->mMeshes[i], scene, dir));
	}
}

void Model::draw(Shader shader) {
	for (auto& mesh : meshes) {
		mesh.draw(shader);
	}
}

void load_textures(std::vector<Texture>& textures, std::string path, aiMaterial* mat, aiTextureType type) {
	for (int i = mat->GetTextureCount(type) - 1; i >= 0; --i) {
		aiString str;
		mat->GetTexture(type, i, &str);

		textures.push_back(TextureManager::load_texture(path + str.C_Str(), false, type));
	}
}

Mesh process_mesh(aiMesh* mesh, const aiScene* scene, std::string path) {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	for (int i = mesh->mNumVertices - 1; i >= 0; --i) {
		Vertex vtx;

		vtx.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vtx.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		if (mesh->mTextureCoords[0]) {
			aiVector3D vec = mesh->mTextureCoords[0][i];
			vtx.texCoords = glm::vec2(vec.x, vec.y);
		} else {
			vtx.texCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vtx);
	}

	for (int i = mesh->mNumFaces - 1; i >= 0; --i) {
		aiFace face = mesh->mFaces[i];

		for (int j = face.mNumIndices - 1; j >= 0; --j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex > 0) {
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		load_textures(textures, path, mat, aiTextureType_DIFFUSE);
		load_textures(textures, path, mat, aiTextureType_SPECULAR);
	}

	return Mesh(vertices, indices, textures);
}
