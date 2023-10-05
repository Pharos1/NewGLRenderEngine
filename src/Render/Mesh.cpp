#include "../pch.h"
#include "Mesh.hpp"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
	create(vertices, indices);
}
Mesh::~Mesh() {
	nLog(std::string("~Mesh has been triggered! Hint: VBO ID -> ") + std::to_string(this->VBO), Log::LogDestructorInfo, "MESH");
	deleteMesh();
}

Mesh::Mesh(Mesh&& other) noexcept {
	std::swap(VBO, other.VBO);
	std::swap(VAO, other.VAO);
	std::swap(EBO, other.EBO);
	std::swap(verticesCount, other.verticesCount);
	std::swap(indicesCount, other.indicesCount);
	std::swap(material, other.material);
}
Mesh& Mesh::operator=(Mesh&& other) noexcept {
	if (this == &other) return *this;

	deleteMesh();

	std::swap(VBO, other.VBO);
	std::swap(VAO, other.VAO);
	std::swap(EBO, other.EBO);
	std::swap(verticesCount, other.verticesCount);
	std::swap(indicesCount, other.indicesCount);
	std::swap(material, other.material);

	return *this;
}

void Mesh::create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
	verticesCount = static_cast<int>(vertices.capacity());
	indicesCount = indices.empty() ? 0 : static_cast<int>(indices.capacity());

	if(!VAO) glGenVertexArrays(1, &VAO);
	if(!VBO) glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	if (indicesCount) {
		if(!EBO) glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	}

	Vertex::setAttribArrays();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void Mesh::deleteMesh() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	VAO = 0;
	VBO = 0;
	EBO = 0;
	verticesCount = 0;
	indicesCount = 0;

	material.deleteMaterial();
}
void Mesh::draw() const {
	material.bind();
	glBindVertexArray(VAO);

	if (indicesCount) {
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCount), GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verticesCount));
	}

	glBindVertexArray(0);
	material.unbind();
}