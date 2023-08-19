#include "../pch.h"
#include "Model.hpp"
#include "../Utilities/Time.hpp"

Model::Model(std::string const& path) {
	loadModel(path);
}

void Model::draw() const {
	for (const Mesh& mesh : meshes) {
		mesh.draw();
	}
}
void Model::loadModel(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_PreTransformVertices | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcessPreset_TargetRealtime_Fast); //aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		mLog(std::string("Failed to load model at path '") + path + "'. ASSIMP error string: " + importer.GetErrorString(), Log::LogError, "MODEL");
		return;
	}
	directory = path.substr(0, path.find_last_of("/\\"));

	meshes.reserve(scene->mNumMeshes);
	loadedTextures.reserve(scene->mNumTextures);

	processNode(scene->mRootNode, scene);
}
void Model::processNode(aiNode* node, const aiScene* scene) {
	for (uint32_t i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene, node));
	}
	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, aiNode* node) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	vertices.reserve(mesh->mNumVertices);
	indices.reserve(mesh->mNumFaces);

	for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.pos = vector;

		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}
		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;

			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoord = vec;

			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;

			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;
		}
		else
			vertex.texCoord = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (uint32_t j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	Mesh finalMesh(vertices, indices);

	if (scene->HasMaterials()) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		loadMaterial(finalMesh.material, 0, material, aiTextureType_DIFFUSE);
		loadMaterial(finalMesh.material, 1, material, aiTextureType_METALNESS);
		loadMaterial(finalMesh.material, 2, material, aiTextureType_DIFFUSE_ROUGHNESS);
		loadMaterial(finalMesh.material, 3, material, aiTextureType_NORMALS);
		//loadMaterial(finalMesh.material.AO, material, aiTextureType_LIGHTMAP);
	}

	return finalMesh;
}
void Model::loadMaterial(Material& meshMaterial, GLuint unit, const aiMaterial* material, aiTextureType type) {
	aiString texturePath;
	if (material->GetTexture(type, 0, &texturePath) == -1)
		return;

	std::string path = this->directory + "/" + texturePath.C_Str();
	std::replace(path.begin(), path.end(), '\\', '/');

	//bool skip = false; //              NO OPTIMIZATION
	//for (int i = 0; i < loadedTextures.size(); i++) {
	//	//auto [loadedPath, loadedID] = loadedTextures[i];
	//	std::string loadedPath = loadedTextures[i].path;
	//	if (loadedPath == path) {
	//		skip = true;
	//
	//		texture = loadedID;
	//
	//		break;
	//	}
	//}
	//if (!skip) {
		//textureID = TextureNS::loadTexture(path.c_str());
	meshMaterial.addTexture(path, unit);
		//loadedTextures.push_back();
	//}
}