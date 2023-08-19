#pragma once
#include "../pch.h"

#include "Mesh.hpp"
#include "../Render/Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "Material.hpp"

class Model {
public:
	std::vector<Texture> loadedTextures;
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