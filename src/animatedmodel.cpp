// You shouldn't read this.
// If you do, it means you're scrolling through my commits.
// Go back and never return. This is not a place for the feeble minds,
// and even Lovecraftian horrors will look like comedy gold after this.

// If you want to continue,
// here's a cute cat to make you happy before you go insane :
//        _
//        \`*-.
//         )  _`-.
//        .  : `. .
//        : _   '  \
//        ; *` _.   `*-._
//        `-.-'          `-.
//          ;       `       `.
//          :.       .        \
//          . \  .   :   .-'   .
//          '  `+.;  ;  '      :
//          :  '  |    ;       ;-.
//          ; '   : :`-:     _.`* ;
//       .*' /  .*' ; .*`- +'  `*'
//       `*-*   `*-*  `*-*'


// It's the last time I warn you,
// go back and keep scrolling as if this commit and file doesn't exists.

// BEGINNING OF THE WORST OPENGL SKELETON CODE YOU HAVE EVER SEEN IN
// 3...
// 2...
// 1...

#include <string>
#include <vector>
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "log.hpp"
#include "animatedmodel.hpp"
#include "animatedmesh.hpp"
#include "shader.hpp"
#include "texture.hpp"

// Yes, this is a copy of Model with only a few things changed.
// @TODO: Find a better way to make animated models
// We only load 1 bone because that's all we need for now
// Just this ^ should tell you the amount of idiocy in this file

#include <glm/gtx/string_cast.hpp>

static inline glm::mat4 aiMatrix4_to_glm_mat4(const aiMatrix4x4& matrix);
static AnimatedMesh process_mesh(
	aiMesh* mesh,
	const aiScene* scene,
	const aiMatrix4x4& transform,
	const std::string& path
);

AnimatedModel::AnimatedModel(const std::string& path) {
	if (!load(path)) {
		throw std::runtime_error("Could not load scene: " + path);
	}
}

glm::mat4 globalinverse, offset;
void AnimatedModel::load_node(const aiScene* scene, aiNode* node, const std::string& path) {
	for (int i = node->mNumMeshes - 1; i >= 0; --i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		meshes.push_back(process_mesh(
			mesh,
			scene,
			node->mTransformation,
			path
		));

		if (mesh->HasBones()) {
			globalinverse = aiMatrix4_to_glm_mat4(node->mTransformation.Inverse());
			offset = aiMatrix4_to_glm_mat4(mesh->mBones[0]->mOffsetMatrix);
		}
	}

	for (int i = node->mNumChildren - 1; i >= 0; --i) {
		load_node(scene, node->mChildren[i], path);
	}
}

bool AnimatedModel::load(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate
		| aiProcess_FlipUVs
		| aiProcess_OptimizeMeshes
		| aiProcess_OptimizeGraph
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		Log::error("Error opening model, ", importer.GetErrorString());
		return false;
	}

	path = path.substr(0, path.find_last_of('/') + 1);

	load_node(scene, scene->mRootNode, path);

	return true;
}

void AnimatedModel::draw(const Shader& shader, const glm::mat4& VP) {
	if (dirty) {
		dirty = false;
		model_mat = glm::translate(glm::mat4(1.0f), pos);
		model_mat = glm::rotate(model_mat, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		bone_mtx = glm::rotate(globalinverse, bone_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		bone_mtx = bone_mtx * offset;
	}

	shader.set_uniform(shader.get_uniform_location("bone"), bone_mtx);

	glm::mat4 MVP = VP * model_mat;
	shader.set_MVP(MVP);

	for (auto& mesh : meshes) {
		mesh.draw(shader);
	}
}

void AnimatedModel::set_bone_angle(float _angle) {
	bone_angle = _angle - angle;
	dirty = true;
}

void AnimatedModel::set_pos(const glm::vec3& pos) {
	this->pos = pos;
	dirty = true;
}

void AnimatedModel::move(const glm::vec3& pos) {
	this->pos += pos;
	dirty = true;
}

void AnimatedModel::set_angle(float angle) {
	this->angle = angle;
	dirty = true;
}

void AnimatedModel::rotate(float angle) {
	this->angle += angle;

	const float full_rotation = glm::pi<float>() * 2.0f;
	if (this->angle > full_rotation || this->angle < -full_rotation) {
		this->angle = glm::mod(this->angle, full_rotation);
	}

	dirty = true;
}

static void load_textures(std::vector<Texture>& textures, const std::string& path, aiMaterial* mat, aiTextureType type) {
	for (int i = mat->GetTextureCount(type) - 1; i >= 0; --i) {
		aiString str;
		mat->GetTexture(type, i, &str);

		textures.push_back(TextureManager::load_texture(path + str.C_Str(), type));
	}
}

static float find_bone_weight(aiMesh* mesh, GLuint vertexID) {
	if (!mesh->HasBones()) {
		return 0.0f;
	}

	aiBone* bone = mesh->mBones[0];

	for (int i = bone->mNumWeights - 1; i >= 0; --i) {
		aiVertexWeight vtx_weight = bone->mWeights[i];

		if (vtx_weight.mVertexId == vertexID) {
			return vtx_weight.mWeight;
		}
	}

	return 0.0f;
}

static AnimatedMesh process_mesh(
		aiMesh* mesh,
		const aiScene* scene,
		const aiMatrix4x4& transform,
		const std::string& path
	) {

	std::vector<BoneVertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	for (int i = mesh->mNumVertices - 1; i >= 0; --i) {
		BoneVertex vtx;

		aiVector3D pos = transform * mesh->mVertices[i];
		aiVector3D normal = transform * mesh->mNormals[i];

		vtx.pos = glm::vec3(pos.x, pos.y, pos.z);
		vtx.normal = glm::vec3(normal.x, normal.y, normal.z);
		vtx.boneWeight = find_bone_weight(mesh, i);

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

	aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
	load_textures(textures, path, mat, aiTextureType_DIFFUSE);
	load_textures(textures, path, mat, aiTextureType_SPECULAR);

	if (textures.empty()) {
		textures.push_back(TextureManager::load_texture("assets/missing_texture.png", aiTextureType_DIFFUSE));
	}

	return AnimatedMesh(vertices, indices, textures);
}

static inline glm::mat4 aiMatrix4_to_glm_mat4(const aiMatrix4x4& matrix) {
    glm::mat4 tmp;
    tmp[0][0] = matrix.a1;
    tmp[1][0] = matrix.b1;
    tmp[2][0] = matrix.c1;
    tmp[3][0] = matrix.d1;

    tmp[0][1] = matrix.a2;
    tmp[1][1] = matrix.b2;
    tmp[2][1] = matrix.c2;
    tmp[3][1] = matrix.d2;

    tmp[0][2] = matrix.a3;
    tmp[1][2] = matrix.b3;
    tmp[2][2] = matrix.c3;
    tmp[3][2] = matrix.d3;

    tmp[0][3] = matrix.a4;
    tmp[1][3] = matrix.b4;
    tmp[2][3] = matrix.c4;
    tmp[3][3] = matrix.d4;
    return tmp;
}
