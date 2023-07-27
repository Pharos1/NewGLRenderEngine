#include "pch.h"
#include "Model.hpp"

Model::Model(std::string const& path) {
	loadModel(path);
}
Model::Model() {};
Model::~Model() {
	mLog(std::string("The destructor of class Model has been triggered. Ensure that all resources are properly handled. Hint: Directory -> ") + this->directory, Log::LogWarning);

	//FINALLY Clenaup
	for (uint32_t i = 0; i < meshes.size(); i++) {
		meshes[i].material.deleteMaterial();
	}
};

void Model::draw(int firstTextureUnit) const {
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(firstTextureUnit);
}
void Model::loadModel(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph); //aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		mLog(std::string("Failed to load model at path '") + path + "'. ASSIMP error string: " + importer.GetErrorString(), Log::LogError);
		return;
	}
	directory = path.substr(0, path.find_last_of("/\\"));

	meshes.reserve(scene->mNumMeshes);
	this->loadedTextures.reserve(scene->mNumTextures);

	processNode(scene->mRootNode, scene);

	loadedTextures.clear(); //Just in case
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
MaterialMesh Model::processMesh(aiMesh* mesh, const aiScene* scene, aiNode* node) {
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

			//vector.x = mesh->mTangents[i].x;
			//vector.y = mesh->mTangents[i].y;
			//vector.z = mesh->mTangents[i].z;
			//vertex.tangent = vector;
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

	MaterialMesh finalMesh(vertices, indices);

	if (scene->HasMaterials()) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		loadMaterial(finalMesh.material.albedo, material, aiTextureType_DIFFUSE);
		loadMaterial(finalMesh.material.normal, material, aiTextureType_HEIGHT);
		loadMaterial(finalMesh.material.metallic, material, aiTextureType_METALNESS);
		loadMaterial(finalMesh.material.roughness, material, aiTextureType_DIFFUSE_ROUGHNESS);
		//loadMaterial(finalMesh.material.AO, material, aiTextureType_LIGHTMAP);
	}

	return finalMesh;
}
void Model::loadMaterial(GLuint& textureID, const aiMaterial* material, aiTextureType type) {
	aiString texturePath;
	if (material->GetTexture(type, 0, &texturePath) == -1)
		return;

	std::string path = this->directory + "/" + texturePath.C_Str();
	std::replace(path.begin(), path.end(), '\\', '/');

	bool skip = false;
	for (int i = 0; i < loadedTextures.size(); i++) {
		auto [loadedPath, loadedID] = loadedTextures[i];
		if (loadedPath == path) {
			skip = true;

			textureID = loadedID;

			break;
		}
	}
	if (!skip) {
		textureID = TextureNS::loadTexture(path.c_str());
		loadedTextures.push_back({ path, textureID });
	}
}