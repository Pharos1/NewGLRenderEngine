#pragma once
#include "pch.h"

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "Material.hpp"

class Model {
public:
	std::vector<std::tuple<std::string, GLuint>> loadedTextures;
	std::vector<MaterialMesh> meshes;
	std::string directory;

	Model(std::string const& path);
	Model();
	~Model();

	void draw(int firstTextureUnit) const;
	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	MaterialMesh processMesh(aiMesh* mesh, const aiScene* scene, aiNode* node);
	void loadMaterial(GLuint& textureID, const aiMaterial* material, aiTextureType type);
};