#pragma once
#include "../pch.h"
#include "Vertex.hpp"
#include "../Render/Material.hpp"

class Mesh{
protected:
	GLuint VBO = 0, VAO = 0, EBO = 0;

public:
	int verticesCount = 0;
	int indicesCount = 0;

	Material material{};

	Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices = {});
	Mesh() = default;
	~Mesh();

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	Mesh(Mesh&& other) noexcept;
	Mesh& operator=(Mesh&& other) noexcept;

	void create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices = {});
	void deleteMesh();
	void draw() const;
};