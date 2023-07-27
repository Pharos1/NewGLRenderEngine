#include "pch.h"
#include "Mesh.hpp"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
		: verticesSize(static_cast<int>(vertices.size())), indicesSize(static_cast<int>(indices.size())) {
	create(&vertices, &indices);
}
Mesh::Mesh(std::vector<Vertex>& vertices)
	: verticesSize(vertices.size()), indicesSize(0) {
	create(&vertices);
}
Mesh::Mesh() {};
Mesh::~Mesh() {
	mLog(std::string("The destructor of class Mesh has been triggered! Ensure that all resources are properly handled. Hint: VAO ID -> ") + std::to_string(VAO), Log::LogWarning);
}
void Mesh::create(std::vector<Vertex>* vertices, std::vector<uint32_t>* indices) {
	verticesSize = static_cast<int>(vertices->size());
	indicesSize = indices ? static_cast<int>(indices->size()) : 0;

	if(!VAO) glGenVertexArrays(1, &VAO);
	if(!VBO) glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), vertices->data(), GL_STATIC_DRAW);

	if (indicesSize) {
		if(!EBO) glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(GLuint), indices->data(), GL_STATIC_DRAW);
	}

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, pos)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void Mesh::draw() const {
	glBindVertexArray(VAO);

	if (indicesSize) {
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesSize), GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verticesSize));
	}
}

void MaterialMesh::draw(int firstTextureUnit) const {
	material.bind(firstTextureUnit);

	glBindVertexArray(VAO);

	if (indicesSize) {
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesSize), GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verticesSize));
	}

	material.unbind(firstTextureUnit);
}
MaterialMesh::~MaterialMesh(){
	mLog(std::string("The destructor of class MaterialMesh has been triggered! Ensure that all resources are properly handled. Hint: VAO ID -> ") + std::to_string(VAO), Log::LogWarning);
}