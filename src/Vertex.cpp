#include "pch.h"
#include "Vertex.hpp"

Vertex::Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texCoord, glm::vec3 tangent)
	: pos(pos), normal(normal), texCoord(texCoord), tangent(tangent) {
}

void Vertex::setAttribArrays() {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, pos)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tangent)));
}

std::ostream& operator<<(std::ostream& os, const glm::vec4& other) {
	os << other.x << ", " << other.y << ", " << other.z << ", " << other.w;
	return os;
}
std::ostream& operator<<(std::ostream& os, const glm::vec3& other) {
	os << other.x << ", " << other.y << ", " << other.z;
	return os;
}
std::ostream& operator<<(std::ostream& os, const glm::vec2& other) {
	os << other.x << ", " << other.y;
	return os;
}