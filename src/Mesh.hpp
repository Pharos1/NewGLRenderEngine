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
	Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	Mesh(std::vector<Vertex>& vertices);
	Mesh();
	~Mesh();
	
	void create(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices = nullptr);
	virtual void draw() const;
};
class MaterialMesh : public Mesh {
public:
	Material material;

	using Mesh::Mesh;
	~MaterialMesh();
	
	void draw(int firstTextureUnit) const;
};