#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "log.hpp"
#include "model.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"

// Meshes loading can be changed to a parent-child system, but we don't need it right now

namespace {
	std::unordered_map<std::string, std::weak_ptr<std::vector<Mesh>>> loaded_models;
}

namespace {
	Mesh process_mesh(aiMesh* mesh, const aiScene* scene, const std::string& path);
}

Model::Model(const std::string& path) {
	if (!load(path)) {
		throw std::runtime_error("Could not load scene: " + path);
	}
}

Model::~Model() {
	if (meshes.use_count() == 1) {
		for (const auto& mesh : *meshes) {
			mesh.clear_gl();
		}

		loaded_models.extract(path);
	}
}

/** @brief Load a COLLADA model file */
bool Model::load(const std::string& path) {
	// Use cached meshes if there's any for this file
	const auto& iter = loaded_models.find(path);
	if (iter != loaded_models.end()) {
		meshes = iter->second.lock();
		return true;
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_Triangulate
		| aiProcess_FlipUVs
		| aiProcess_OptimizeMeshes
		| aiProcess_OptimizeGraph
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		Log::error("Error opening model at ", path, ":\n", importer.GetErrorString());
		return false;
	}

	meshes = std::make_shared<std::vector<Mesh>>();

	std::string path_dir = path.substr(0, path.find_last_of('/') + 1);

	for (int i = scene->mNumMeshes - 1; i >= 0; --i) {
		meshes->push_back(process_mesh(scene->mMeshes[i], scene, path_dir));
	}

	loaded_models.emplace(path, meshes); // Cache meshes
	this->path = path;

	return true;
}

void Model::draw(const Shader& shader, const glm::mat4& VP) {
	if (dirty) {
		dirty = false;
		model_mat = glm::translate(glm::mat4(1.0f), pos);
		model_mat = glm::rotate(model_mat, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::mat4 MVP = VP * model_mat;
	shader.set_MVP(MVP);

	if (!is_empty()) {
		for (const auto& mesh : *meshes) {
			mesh.draw(shader);
		}
	}
}

void Model::set_pos(const glm::vec3& pos) {
	this->pos = pos;
	dirty = true;
}

void Model::move(const glm::vec3& pos) {
	this->pos += pos;
	dirty = true;
}

void Model::set_angle(float angle) {
	this->angle = angle;
	dirty = true;
}

void Model::rotate(float angle) {
	this->angle += angle;

	const float full_rotation = glm::pi<float>() * 2.0f;
	if (this->angle > full_rotation || this->angle < -full_rotation) {
		this->angle = glm::mod(this->angle, full_rotation);
	}

	dirty = true;
}

namespace {
	/**
	 * @brief Load textures in a textures vector
	 *
	 * @param[out] textures The vector to put textures into
	 * @param[in] path Directory path to where the textures are located
	 * @param[in] mat The aiMaterial to get textures from
	 * @param[in] type Type of textures wanted
	 */
	inline void load_textures(
			std::vector<Texture>& textures,
			const std::string& path,
			aiMaterial* mat,
			aiTextureType type
		) {

		for (int i = mat->GetTextureCount(type) - 1; i >= 0; --i) {
			aiString str;
			mat->GetTexture(type, i, &str);

			textures.push_back(TextureManager::load_texture(path + str.C_Str(), type));
		}
	}

	/**
	 * @brief Load a mesh and its components
	 *
	 * Components can include indices, vertices, normals...
	 *
	 * @param mesh The mesh to process
	 * @param scene The scene where the mesh is located
	 * @param path Path to the model file's directory
	 *
	 * @return Fully initialized Mesh object for the provided mesh
	 */
	Mesh process_mesh(aiMesh* mesh, const aiScene* scene, const std::string& path) {
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		for (int i = mesh->mNumVertices - 1; i >= 0; --i) {
			Vertex vtx;

			aiVector3D pos = scene->mRootNode->mTransformation * mesh->mVertices[i];
			// @TODO: Remove normals
			aiVector3D normal = scene->mRootNode->mTransformation * mesh->mNormals[i];

			vtx.pos = glm::vec3(pos.x, pos.y, pos.z);
			vtx.normal = glm::vec3(normal.x, normal.y, normal.z);

			if (mesh->mTextureCoords[0]) { // if the mesh has textures
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

		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		load_textures(textures, path, mat, aiTextureType_DIFFUSE);
		load_textures(textures, path, mat, aiTextureType_SPECULAR);

		if (textures.empty()) {
			textures.push_back(
				TextureManager::load_texture("assets/missing_texture.png", aiTextureType_DIFFUSE)
			);
		}

		return Mesh(vertices, indices, textures);
	}
}
