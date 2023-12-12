#pragma once
#include "../pch.h"

#include "Mesh.hpp"
#include "../GL/Shader.hpp"
#include "../GL/Texture.hpp"
#include "Vertex.hpp"
#include "../Object/Material.hpp"

class Model {
public:
	std::vector<std::pair<Texture*, std::string>> loadedTextures;
	std::vector<Mesh> meshes;
	std::string directory;

	Model(std::string const& path);
	Model() = default;

	void draw() const;
	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, aiNode* node);
	void loadMaterial(Material& meshMaterial, GLuint unit, const aiMaterial* material, aiTextureType type);
};