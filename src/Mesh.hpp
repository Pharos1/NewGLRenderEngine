#pragma once
#include "pch.h"
#include "Vertex.hpp"
#include "Material.hpp"

class Mesh{
protected:
	GLuint VBO = 0, VAO = 0, EBO = 0;

	int verticesSize = 0;
	int indicesSize = 0;

public:
	Material material{};

	Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices = {});
	Mesh() = default;

	void create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices = {});
	void deleteMesh();
	void draw(uint32_t firstTextureUnit = 0) const;
};