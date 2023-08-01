#include "pch.h"
#include "Mesh.hpp"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		: verticesSize(static_cast<int>(vertices.size())), indicesSize(static_cast<int>(indices.size())) {
	create(vertices, indices);
}

void Mesh::create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
	verticesSize = static_cast<int>(vertices.size());
	indicesSize = indices.empty() ? 0 : static_cast<int>(indices.size());

	if(!VAO) glGenVertexArrays(1, &VAO);
	if(!VBO) glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	if (indicesSize) {
		if(!EBO) glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	}

	Vertex::setAttribArrays();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void Mesh::deleteMesh() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	verticesSize = 0;
	indicesSize = 0;

	material.deleteMaterial();
}
void Mesh::draw(uint32_t firstTextureUnit) const {
	if (!material.empty()) {
		material.bind(firstTextureUnit);
	}
	glBindVertexArray(VAO);

	if (indicesSize) {
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesSize), GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verticesSize));
	}

	glBindVertexArray(0);

	if (!material.empty()) {
		material.unbind(firstTextureUnit);
	}
}