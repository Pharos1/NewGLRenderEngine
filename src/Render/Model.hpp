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
	std::mutex mLock;
	std::vector<std::future<void>> futures;

	Model(std::string const& path);
	Model() = default;

	void draw(int firstTextureUnit = 0) const;
	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, aiNode* node);
	void loadMaterial(Texture& texture, const aiMaterial* material, aiTextureType type);
};