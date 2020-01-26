#include <string>
#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "errors.hpp"
#include "model.hpp"
#include "mesh.hpp"
#include "shaders.hpp"
#include "texture.hpp"

// Meshes loading can be changed to a parent-child system, but we don't need it right now

void loadTextures(std::vector<Texture>& textures, std::string path, aiMaterial* mat, aiTextureType type);
Mesh processMesh(aiMesh* assMesh, const aiScene* scene, std::string path);

Model::Model(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		PRINT_ERROR("Error opening model, " << importer.GetErrorString());
		return;
	}

	std::string dir = path.substr(0, path.find_last_of('/') + 1);

	for (int i = scene->mNumMeshes - 1; i >= 0; --i) {
		mMeshes.push_back(processMesh(scene->mMeshes[i], scene, dir));
	}
}

void Model::draw(Shader shader) {
	for (auto& mesh : mMeshes) {
		mesh.draw(shader);
	}
}

void loadTextures(std::vector<Texture>& textures, std::string path, aiMaterial* mat, aiTextureType type) {
	for (int i = mat->GetTextureCount(type) - 1; i >= 0; --i) {
		aiString str;
		mat->GetTexture(type, i, &str);

		textures.push_back(TextureManager::loadTexture(path + str.C_Str(), false, type));
	}
}

Mesh processMesh(aiMesh* assMesh, const aiScene* scene, std::string path) {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	for (int i = assMesh->mNumVertices - 1; i >= 0; --i) {
		Vertex vtx;

		vtx.pos = glm::vec3(assMesh->mVertices[i].x, assMesh->mVertices[i].y, assMesh->mVertices[i].z);
		vtx.normal = glm::vec3(assMesh->mNormals[i].x, assMesh->mNormals[i].y, assMesh->mNormals[i].z);

		if (assMesh->mTextureCoords[0]) {
			aiVector3D vec = assMesh->mTextureCoords[0][i];
			vtx.texCoords = glm::vec2(vec.x, vec.y);
		} else {
			vtx.texCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vtx);
	}

	for (int i = assMesh->mNumFaces - 1; i >= 0; --i) {
		aiFace face = assMesh->mFaces[i];

		for (int j = face.mNumIndices - 1; j >= 0; --j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (assMesh->mMaterialIndex > 0) {
		aiMaterial* mat = scene->mMaterials[assMesh->mMaterialIndex];
		loadTextures(textures, path, mat, aiTextureType_DIFFUSE);
		loadTextures(textures, path, mat, aiTextureType_SPECULAR);
	}

	return Mesh(vertices, indices, textures);
}
